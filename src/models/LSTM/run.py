#!/usr/bin/env python
# coding: utf-8

# # Import Statements

# In[1]:


import os
os.chdir('../../..')
import warnings
warnings.filterwarnings("ignore")

import pandas as pd
import numpy as np
import datetime
from time import time

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder

import tensorflow as tf
from keras.preprocessing import sequence
from keras.models import Sequential
from keras.layers import Dense, LSTM, TimeDistributed
from sklearn.metrics import accuracy_score, balanced_accuracy_score, f1_score, precision_score, recall_score

import plotly.graph_objects as go


# # Data Cleaning

# In[2]:


def clean_data(df):
    # convert to datetime
    df['MEASUREMENT_TIME'] = pd.to_datetime(df['MEASUREMENT_TIME'], errors='coerce')
    # calculate time difference
    df['diff'] = df['MEASUREMENT_TIME'].diff().shift(-1)
    # drop unncessary columns
    df = df.drop(columns=['ID_INPUT', 'PRIVATE_DATA'])
    # rename remaining columns
    df.columns = ['time', 'window', 'diff']
    # Select 3-week time-frame
#     df = df[(df['time'] >= '2022-01-03') & (df['time'] <= '2022-01-24')]
    # Remove windows appearing only once
    df = df.groupby('window').filter(lambda x: len(x) > 1)
    # Remove NaNs
    df = df.dropna()

    return df


# # Time-Series Regularization

# In[3]:


def regularize_timeseries(df, freq):
    # regularized time-series index at specified frequency
    out_time = pd.date_range(df['time'].values[0], df['time'].values[len(df)-1], freq=freq)
    out_windows = []

    for i in range(len(out_time[:-1])):
        # subquery dataframe to each time-step
        df_small = df[(df['time'] >= out_time[i]) & (df['time'] <= out_time[i+1])]

        if len(df_small) == 0:
            # NaN if no windows in time-step
            out_windows.append(np.NaN)
        elif len(df_small) == 1:
            # append window if only one window in time-step
            out_windows.append(df_small['window'].values[0])
        else:
            # append window with most time spent if multiple windows during time-step
            summed = df_small.groupby('window')['diff'].sum().reset_index().sort_values('diff', ascending=False)
            out_windows.append(summed['window'].values[0])

    # create new dataframe
    out = pd.DataFrame(list(zip(out_time, out_windows)), columns =['time', 'window']).fillna(method="ffill")

    return out.set_index('time')


# # Feature Engineering

# In[4]:


def one_hot_encode(sequence, n_unique):
    """one hot encode a sequence as 2-d array"""
    encoding = list()
    for value in sequence:
        vector = [0 for _ in range(n_unique)]
        vector[value] = 1
        encoding.append(vector)
    return np.array(encoding)


# In[5]:


def to_supervised(sequence, n_in, n_out):
    """transform encoded sequence to supervised learning problem"""
    # create lag copies of the sequence
    df = pd.DataFrame(sequence)
    df = pd.concat([df.shift(n_in-i-1) for i in range(n_in)], axis=1)
    # drop rows with missing values
    df.dropna(inplace=True)
    # specify columns for input and output pairs
    values = df.values
    width = sequence.shape[1]
    X = values.reshape(len(values), n_in, width)
    y = values[:, 0:(n_out*width)].reshape(len(values), n_out, width)

    return X, y


# In[6]:


def one_hot_decode(encoded_seq):
    """decode a one hot encoded string"""
    return [np.argmax(vector) for vector in encoded_seq]


# In[7]:


def decode_predictions(pred, test, dim, enc):
    """decode all one-hot encoded strings and store as dataframe"""
    preds = []
    for i in range(len(test[dim-1:])):
        preds.append(one_hot_decode(pred[i])[0])

    return pd.DataFrame(enc.inverse_transform(preds), index=test.index[dim-1:], columns=['window'])


# # Plots

# In[8]:


def make_time_series_plot(df_train, df_valid, df_test, df_pred, freq, mode='markers'):
    """Final App Launch+Usage Predictions / Total Time Spent"""
    trace1 = go.Scatter(
        x = df_train.index,
        y = df_train.window,
        mode = mode,
        marker = dict(color = 'royalblue'),
        name = 'Training Data'
    )
    trace2 = go.Scatter(
        x = df_valid.index,
        y = df_valid.window,
        mode = mode,
        marker = dict(color = 'mediumpurple'),
        name = 'Validation Data'
    )
    trace3 = go.Scatter(
        x = df_test.index,
        y = df_test.window,
        mode = mode,
        marker = dict(color = 'red'),
        name = 'Testing Data'
    )
    trace4 = go.Scatter(
        x = df_pred.index,
        y = df_pred.window,
        mode = mode,
        marker = dict(color = 'mediumseagreen'),
        name = 'Predictions'
    )
    layout = go.Layout(
        title = "App Usage Predictions at {} Intervals".format(freq),
        xaxis = {'title' : "Time"},
        yaxis = {'title' : "App Executable"}
    )

    return go.Figure(data=[trace1, trace2, trace3, trace4], layout=layout)


# # LSTM Model

# In[9]:


def evaluate_all_models():
    #list users
    users = ['user1', 'user2', 'intel']
    #regularization frequencies
    freqs = ['10s', '30s', '1min']
    #hyper-parameters
    look_backs = [3, 6, 12]
    nodes = [16, 32, 64]
    batch_sizes = [6, 12, 24]
    for user in users:
        #number of models tested
        count = 1
        #outputs dictionary
        outputs = {'frequency': [], 'look_back':[], 'num_nodes':[], 'batch_size':[],
                   'accuracy':[], 'balanced_accuracy':[], 'weighted_f1_score':[],
                   'weighted_precision':[], 'weighted_recall':[], 'time': []}
        #load data for user
        df = pd.read_csv('data/{}_window_data.csv'.format(user))
        #clean user window data
        df = clean_data(df)
        for freq in freqs:
            #regularize time-series by frequency
            df_regular = regularize_timeseries(df, freq)
            #split regular data into train and test for later time-series evaluation
            #60-20-20 split for train-validation-test
            df_train_regular, df_test_regular = train_test_split(df_regular, test_size=0.4, shuffle=False)
            df_valid_regular, df_test_regular = train_test_split(df_test_regular, test_size=0.5, shuffle=False)
            #transform categorical labels to numerical
            enc = LabelEncoder()
            df_labeled = enc.fit_transform(df_regular)
            encoded_length = len(enc.classes_)
            #one-hot encode numerical labels
            df_encoded = one_hot_encode(df_labeled, encoded_length)
            #split one-hot encoded arrays into train and test
            #60-20-20 split for train-validation-test
            df_train, df_test = train_test_split(df_encoded, test_size=0.4, shuffle=False)
            df_valid, df_test = train_test_split(df_test, test_size=0.5, shuffle=False)
            for look_back in look_backs:
                #specify different look_back values for sequence input and prediction length
                #convert to supervised learning problem
                X_train, y_train = to_supervised(df_train, look_back, look_back)
                X_valid, y_valid = to_supervised(df_valid, look_back, look_back)
                X_test, y_test = to_supervised(df_test, look_back, look_back)
                #specify different number of nodes in LSTM layer
                for node in nodes:
                    #specify different batch sizes for model fitting
                    for batch_size in batch_sizes:
                        #instantiate Sequential model
                        model = Sequential()
                        model.add(LSTM(node, input_shape=(look_back, encoded_length), return_sequences=True))
                        model.add(TimeDistributed(Dense(encoded_length, activation='softmax')))
                        model.compile(optimizer='adam',
                                      loss='categorical_crossentropy',
                                      metrics='categorical_accuracy')
                        #optimal epochs selected based on epoch-loss curve
                        start = time()
                        history = model.fit(X_train, y_train,
                                  epochs=10,
                                  batch_size=batch_size,
                                  validation_data=(X_valid, y_valid),
                                  verbose=0,
                                  shuffle=False)
                        #save model training logs in dataframe
                        end = time()
                        log = pd.DataFrame(history.history)
                        log.to_csv("outputs/LSTM/tables/{}_model_{}_logs.csv".format(user,count))
                        #make predictions on test data
                        y_pred = model.predict(X_test, batch_size=batch_size, verbose=0)
                        df_pred = decode_predictions(y_pred, df_test_regular, look_back, enc)
                        #evaluate predictions
                        accuracy = accuracy_score(df_pred, df_test_regular[look_back-1:])
                        balanced_accuracy = balanced_accuracy_score(df_pred, df_test_regular[look_back-1:])
                        f1 = f1_score(df_pred, df_test_regular[look_back-1:], average='weighted')
                        precision = precision_score(df_pred, df_test_regular[look_back-1:], average='weighted')
                        recall = recall_score(df_pred, df_test_regular[look_back-1:], average='weighted')
                        #save parameters and evaluations
                        outputs['frequency'].append(freq)
                        outputs['look_back'].append(look_back)
                        outputs['num_nodes'].append(node)
                        outputs['batch_size'].append(batch_size)
                        outputs['accuracy'].append(accuracy)
                        outputs['balanced_accuracy'].append(balanced_accuracy)
                        outputs['weighted_f1_score'].append(f1)
                        outputs['weighted_precision'].append(precision)
                        outputs['weighted_recall'].append(recall)
                        outputs['time'].append(end-start)
                        #save figures
                        fig = make_time_series_plot(df_train_regular, df_valid_regular, df_test_regular,
                                                    df_pred, freq)
                        fig.write_image("outputs/LSTM/plots/{}_model_{}_dot_plot.png".format(user,count))
                        fig = make_time_series_plot(df_train_regular, df_valid_regular, df_test_regular,
                                                    df_pred, freq, mode='lines')
                        fig.write_image("outputs/LSTM/plots/{}_model_{}_line_plot.png".format(user,count))
                        #increment model count
                        print(count, end='\r')
                        count+=1

        pd.DataFrame(outputs).to_csv("outputs/LSTM/tables/{}_model_outputs.csv".format(user))


# In[10]:

evaluate_all_models()
