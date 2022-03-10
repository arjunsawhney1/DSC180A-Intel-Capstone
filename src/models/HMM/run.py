import pandas as pd
from HMM import HMM

xda = HMM()
import os
os.chdir('../../..')
import warnings
warnings.filterwarnings("ignore")
import plotly.graph_objects as go
import numpy as np
import json

# read in json file with user data
def get_params():
    users=json.loads('config/hyperparameters.json')['users']
    

def hmm_predict(train_data, test_data):
    num_correct = 0
    tp_1=xda.predictor1(train_data, test_data)
    predicted_window_sequence = xda.predictor2(train_data, test_data)
    for i in range(len(test_data.window) - 1):
        if test_data.window.iloc[i] == predicted_window_sequence[i]:
            num_correct += 1
    tp_2=num_correct / len(test_data)
    tp_3=xda.predictor3(train_data, test_data)
    return [tp_1,tp_2,tp_3]

def test():
    print("Start Pipeline:")
    user='user1'
    data_us1 = xda.clean_data('data/{}_window_data.csv'.format(user),
                         "data/{}_immersive_data.csv".format(user))
    user='user2'
    data_us2 = xda.clean_data('data/{}_window_data.csv'.format(user),
                            "data/{}_immersive_data.csv".format(user))
    train_data_1,test_data_1 = xda.train_test_split(data_us1)
    train_data_2,test_data_2 = xda.train_test_split(data_us2)
    accuracy=[hmm_predict(train_data_1,test_data_1)[0],hmm_predict(train_data_2,test_data_2)[0],hmm_predict(train_data_1,test_data_1)[1],hmm_predict(train_data_2,test_data_2)[1],hmm_predict(train_data_1,test_data_1)[2],hmm_predict(train_data_2,test_data_2)[2]]
    dict={'Prediction':['Pred1','Pred1','Pred2','Pred2','Pred3','Pred3'],
                   'Accuracy':accuracy,
                   'User':['User1','User2','User1','User2','User1','User2']}
    print("End Pipeline:")
    return dict

def plots():
    print("Start Plots:")
    acc_df=pd.DataFrame(test())
    user_1=acc_df[acc_df['User']=='User1']
    user_2=acc_df[acc_df['User']=='User2']
    fig = go.Figure(data=[go.Bar(x=user_1['Prediction'], y=user_1['Accuracy'],
                                marker_color='blue')])
    fig.update_layout(title='User1 Accuracy per Predictors',
                    xaxis_title='Predictors',
                    yaxis_title='Accuracy')
    fig.write_image('outputs/HMM/plots/user1_accuracy.png')
    fig = go.Figure(data=[go.Bar(x=user_2['Prediction'], y=user_2['Accuracy'],
                                marker_color='blue')])
    fig.update_layout(title='User2 Accuracy per Predictors',
                    xaxis_title='Predictors',
                    yaxis_title='Accuracy')
    fig.write_image('outputs/HMM/plots/user2_accuracy.png')
    print("End Plots:")

def tables():
    results = pd.DataFrame(test())
    results.to_csv('outputs/HMM/tables/accuracies.csv')

tables()
plots()
