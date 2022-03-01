#!/usr/bin/env python
# coding: utf-8

# In[1]:


import os
os.chdir('../..')
import warnings
warnings.filterwarnings("ignore")

import pandas as pd
import numpy as np

import plotly.express as px
import plotly.graph_objects as go


# # Load Data

# ## User 1

# In[12]:


user1_outputs = pd.read_csv('outputs/LSTM/tables/user1_model_outputs.csv')
user1_outputs = user1_outputs.drop(columns=['Unnamed: 0', 'weighted_f1_score', 'weighted_precision', 'weighted_recall'])
user1_outputs['Model'] = user1_outputs.index + 1
user1_outputs = user1_outputs.set_index('Model')
user1_outputs


# In[13]:


user1_top5_accuracy = user1_outputs.sort_values(['accuracy'], ascending=False).head(5)
user1_top5_accuracy


# In[14]:


user1_top5_balanced_accuracy = user1_outputs.sort_values(['balanced_accuracy'], ascending=False).head(5)
user1_top5_balanced_accuracy


# ## User 2

# In[15]:


user2_outputs = pd.read_csv('outputs/LSTM/tables/user2_model_outputs.csv')
user2_outputs = user2_outputs.drop(columns=['Unnamed: 0', 'weighted_f1_score', 'weighted_precision', 'weighted_recall'])
user2_outputs['Model'] = user2_outputs.index + 1
user2_outputs = user2_outputs.set_index('Model')
user2_outputs


# In[16]:


user2_top5_accuracy = user2_outputs.sort_values(['accuracy'], ascending=False).head(5)
user2_top5_accuracy


# In[17]:


user2_top5_balanced_accuracy = user2_outputs.sort_values(['balanced_accuracy'], ascending=False).head(5)
user2_top5_balanced_accuracy


# # Accuracy Plots by Frequency, Parameter

# ## User 1

# ### look_back

# In[18]:


check = user1_outputs.groupby(['frequency', 'look_back']).mean().reset_index()
check


# In[19]:


fig = px.line(check, x="look_back", y="accuracy", color='frequency', title="User1 Average Accuracy by frequency, look_back")
fig.show()


# In[20]:


fig = px.line(check, x="look_back", y="balanced_accuracy", color='frequency', title="User1 Average Balanced Accuracy by frequency, look_back")
fig.show()


# ### num_nodes

# In[21]:


check = user1_outputs.groupby(['frequency', 'num_nodes']).mean().reset_index()
check


# In[22]:


fig = px.line(check, x="num_nodes", y="accuracy", color='frequency', title="User1 Average Accuracy by frequency', 'num_nodes")
fig.show()


# In[23]:


fig = px.line(check, x="num_nodes", y="balanced_accuracy", color='frequency', title="User1 Average Balanced Accuracy by frequency, num_nodes")
fig.show()


# ### batch_size

# In[24]:


check = user1_outputs.groupby(['frequency', 'batch_size']).mean().reset_index()
check


# In[25]:


fig = px.line(check, x="batch_size", y="accuracy", color='frequency', title="User1 Average Accuracy by frequency, batch_size")
fig.show()


# In[26]:


fig = px.line(check, x="batch_size", y="balanced_accuracy", color='frequency', title="User1 Average Balanced Accuracy by frequency, batch_size")
fig.show()


# ## User 2

# ### look_back

# In[ ]:


check = user2_outputs.groupby(['frequency', 'look_back']).mean().reset_index()
check


# In[ ]:


fig = px.line(check, x="look_back", y="accuracy", color='frequency', title="User2 Average Accuracy by frequency, look_back")
fig.show()


# In[ ]:


fig = px.line(check, x="look_back", y="balanced_accuracy", color='frequency', title="User2 Average Balanced Accuracy by frequency, look_back")
fig.show()


# ### num_nodes

# In[ ]:


check = user2_outputs.groupby(['frequency', 'num_nodes']).mean().reset_index()
check


# In[ ]:


fig = px.line(check, x="num_nodes", y="accuracy", color='frequency', title="User2 Average Accuracy by frequency', 'num_nodes")
fig.show()


# In[ ]:


fig = px.line(check, x="num_nodes", y="balanced_accuracy", color='frequency', title="User2 Average Balanced Accuracy by frequency, num_nodes")
fig.show()


# ### batch_size

# In[ ]:


check = user2_outputs.groupby(['frequency', 'batch_size']).mean().reset_index()
check


# In[ ]:


fig = px.line(check, x="batch_size", y="accuracy", color='frequency', title="User2 Average Accuracy by frequency, batch_size")
fig.show()


# In[ ]:


fig = px.line(check, x="batch_size", y="balanced_accuracy", color='frequency', title="User2 Average Balanced Accuracy by frequency, batch_size")
fig.show()


# # Training & Validation Plots for Top Models

# ## User 1

# In[30]:


for model in user1_top5_accuracy.index[0:1]:
    log = pd.read_csv('outputs/LSTM/tables/user1_model_{}_logs.csv'.format(model))
    log['Epoch'] = log['Unnamed: 0']
    log = log.drop(columns=['Unnamed: 0'])
    
    fig = px.line(log, x="Epoch", y=["categorical_accuracy", "val_categorical_accuracy"], 
                  title="User1 Training & Validation Categorical Accuracy by Epoch")
    fig.show()
    
    fig = px.line(log, x="Epoch", y=["loss", "val_loss"], 
                  title="User1 Training & Validation Categorical CrossEntropy Loss by Epoch")
    fig.show()


# In[ ]:


for model in user1_top5_balanced_accuracy.index:
    log = pd.read_csv('outputs/LSTM/tables/user1_model_{}_logs.csv'.format(model))
    log['Epoch'] = log['Unnamed: 0']
    log = log.drop(columns=['Unnamed: 0'])
    
    fig = px.line(log, x="Epoch", y=["categorical_accuracy", "val_categorical_accuracy"], 
                  title="User1 Training & Validation Categorical Accuracy by Epoch")
    fig.show()
    
    fig = px.line(log, x="Epoch", y=["loss", "val_loss"], 
                  title="User1 Training & Validation Categorical CrossEntropy Loss by Epoch")
    fig.show()


# ## User 2

# In[32]:


for model in user2_top5_accuracy.index[0:1]:
    log = pd.read_csv('outputs/LSTM/tables/user2_model_{}_logs.csv'.format(model))
    log['Epoch'] = log['Unnamed: 0']
    log = log.drop(columns=['Unnamed: 0'])
    
    fig = px.line(log, x="Epoch", y=["categorical_accuracy", "val_categorical_accuracy"], 
                  title="User2 Training & Validation Categorical Accuracy by Epoch")
    fig.show()
    
    fig = px.line(log, x="Epoch", y=["loss", "val_loss"], 
                  title="User2 Training & Validation Categorical CrossEntropy Loss by Epoch")
    fig.show()


# In[ ]:


for model in user2_top5_balanced_accuracy['Model'][0:1]:
    log = pd.read_csv('outputs/LSTM/tables/user2_model_{}_logs.csv'.format(model))
    log['Epoch'] = log['Unnamed: 0']
    log = log.drop(columns=['Unnamed: 0'])
    
    fig = px.line(log, x="Epoch", y=["categorical_accuracy", "val_categorical_accuracy"], 
                  title="User2 Training & Validation Categorical Accuracy by Epoch")
    fig.show()
    
    fig = px.line(log, x="Epoch", y=["loss", "val_loss"], 
                  title="User2 Training & Validation Categorical CrossEntropy Loss by Epoch")
    fig.show()


# # Desktop Mapper

# In[ ]:


df = pd.read_csv('data/dmapper_data.csv')
df


# In[ ]:


df = df[df['curr_win'] != df['next_win']]
df = df[df['curr_win'] != df['prev_win']]
df = df[df['curr_win'].shift(4) != df['curr_win']]
df = df[df['curr_win'].shift(3) != df['curr_win']]
df = df[df['curr_win'].shift(2) != df['curr_win']]
df = df[df['curr_win'].shift(1) != df['curr_win']]
df = df.drop(columns=['private_data'])
df


# In[ ]:


df.shift(-3)

