import pandas as pd
import math

class HMM:
    """
    Init Function; temp value of an empty list
    @param  
    @return 
    """
    def __init__(self):
        self.temp = []
        
    """
    Helper function that extracts list of unique windows that appear
    after given win
    @param  win str window to check
            df df to check from
    @return list of windows after
    """    
    def get_next_window(self, win, df):
        windows_after_win = []
        for i in range(len(df) - 1):
            if df['window'].iloc[i] == win:
                windows_after_win.append(df['window'].iloc[i+1])
        return windows_after_win

    """
    Helper function that outputs a list of size = num of unique windows.
    Elements are the probabilities that index's window will appear after the
    window being checked.
    Indces are the index of the window_order.
    @param  win str window to check
            df df to check from
    @return list of float probabilities 
    """ 
    def get_cond_probs(self, win, df):
        temp_windows = pd.Series(self.get_next_window(win, df))
        return temp_windows.value_counts() / temp_windows.value_counts().sum()

    """
    Outputs nested list of an nxn matrix where n = len(window_order).
    Contents are the probability of column window will appear after row
    window.
    Indexes are window_order.
    @param  data df to extract windows from
    @return list of probability float lists
    """ 
    def transition_matrix(self,data):
        window_order = list(data.window.unique())
        trans_matrix = []
        for curr_win in window_order:
            curr_row  = [0] * len(window_order)
            probs = self.get_cond_probs(curr_win, data)
            probs_dict = dict(probs)
            for win in probs.index:
                prob = probs_dict[win]
                i = window_order.index(win)
                curr_row[i] = prob
            trans_matrix.append(curr_row)
        return trans_matrix

    """
    Extracts the probability the window will be immersive. 
    Calculated by getting num immersive = 1 and dividing that by
    the number of occurences for that window
    @param  win str window to check
            df df to check from
    @return float percentage probability
    """ 
    def get_immersive_prob(self, win, data):
        win_data = data[data.window == win]
        total_is_immersive = win_data.is_immersive.value_counts().sum() #this doesn't count values of NaN
        num_immersive = len(win_data[win_data.is_immersive == 1])
        return num_immersive / total_is_immersive 

    """
    Outputs nested list of an 1xn matrix where n = len(window_order).
    Contents are the probability that column (window) is immersive
    Indexes are window_order.
    @param  data df to extract windows from
    @return list of probability float lists
    """
    def emission_matrix(self,data):
        window_order = list(data.window.unique())
        emis_matrix = []
        for win in window_order:
            emis_matrix.append(self.get_immersive_prob(win, data))
        emis_matrix = [0.0 if math.isnan(x) else x for x in emis_matrix]
        return [emis_matrix]  

    """
    Cleans csv and outputs either a windows df OR windows and is_immersive df
    @params: window_path str csv path to immersive data
             imm_path 1 denotes that there is no immersive data inputed
                      input a str to tell code that there is immersive data to clean as well
    @return: df clean dataframe with proper formatting
    """
    def clean_data(self, window_path, imm_path = 1): 
        #if immersive data is not provided
        if type(imm_path) == int:
            data = pd.read_csv(window_path)
            #check and clean headers
            if data.columns[0] == 'MEASUREMENT_TIME':
                data = data.drop(['ID_INPUT','PRIVATE_DATA'],axis = 1)
                data.columns = ['time','window']
        #if immersive data IS provided
        else:
            window_data = pd.read_csv(window_path)
            immersive_data = pd.read_csv(imm_path)
            #check and clean headers
            if window_data.columns[0] == 'MEASUREMENT_TIME':
                window_data = window_data.drop(['ID_INPUT','PRIVATE_DATA'],axis = 1)
                window_data.columns = ['time','window']
            if immersive_data.columns[0] == 'MEASUREMENT_TIME':
                immersive_data = immersive_data.drop(['ID_INPUT','PRIVATE_DATA'],axis = 1)
                immersive_data.columns = ['time','is_immersive']
                
            data = window_data.merge(immersive_data, on = "time", how = 'left')
            #clean is_immersive column
            #get specific nan float value
            is_NaN = data.isnull()
            row_has_NaN = is_NaN.any(axis=1)
            rows_with_NaN = data[row_has_NaN]
            #in case there are no nan values
            if len(rows_with_NaN != 0):
                nan_value = rows_with_NaN.iloc[0,0]
            else:
                nan_value = '_'
            #get rid of any number other than 0, 1 or nan value
            data = data[data.is_immersive.isin([0,1,nan_value])]

        #change time col into a datetime object
        data['time'] = pd.to_datetime(data['time'])    
        return data

    """
    Splits data into train and test portions
    Assumes data is already cleaned
    Training data is all data except last recorded day's data
    Test data is the last recorded day's data
    @params: data df to split 
    @return: 2 dfs (train and test dfs)
    """
    def train_test_split(self,data):
        last_month = data.iloc[-1].time.month
        last_day = data.iloc[-1].time.day
        #returns boolean if day matches with target date, for every column
        def bool_day_checker(date):
            if date.month == last_month and date.day == last_day:
                return True
            return False
        last_day_data = data.loc[data.time.apply(bool_day_checker)]
        last_index = last_day_data.index[0]
        train_data = data.loc[:last_index - 1] #-1 since loc ending is inclusive
        test_data = last_day_data
        return train_data, test_data

    """
    Traditional HMM Predictor
    This predictor outputs the top x probable windows 
    and checks the current test data window. if it's in the top x.
    If it is not in the threshold, then a check mark of incorrect is added.
    Accuracy is the ouptut.
    @param  train df to train predictor on
            test df to test predictor on
    @return float accuracy 
    """
    def predictor1(self,train, test):
        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing) ~ made on train data
        trans_matrix = pd.DataFrame(self.transition_matrix(train))
        trans_matrix.columns = window_order
        trans_matrix.index = window_order
        
        num_correct = 0
        
        #Start from the very first window
        for i in range(len(test.window) - 1): # -1 to not go past the last row
            win = test.window.iloc[i]
            next_win = test.window.iloc[i+1]
            
            #make sure win is an index of the matrix
            if win in window_order:
                win_index = window_order.index(win)
                probs = trans_matrix.loc[win]
                #setting it as top 5, will be change d depending on accuracy
                top_prob_wins = probs.sort_values(ascending = False)[:6].index
                #check if the next window is within the threshold
                if next_win in top_prob_wins:
                    num_correct +=1
            # if window is NOT in matrix, check to see if next window is part of the top 5 most common windows
            # if so, then its a correct prediction
            else:
                top_windows = train.window.value_counts(ascending = False)[:5].index
                if next_win in top_windows:
                    num_correct +=1
                    
        return num_correct / (len(test.window) - 1) # -1 so that we dont guess the last window's 


    """
    Sequence HMM Predictor
    This predictor ouputs a sequence of next windows. 
    Accuracy is measured AFTER and checks the current test data window. if it's in the top x.
    If it is not in the threshold, then a check mark of incorrect is added.
    @param  train df to train predictor on
            test df to test predictor on
    @return list of strings (windows) to compare to test sequence of windows 
    """
    def predictor2(self,train,test):
        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing)
        trans_matrix = pd.DataFrame(self.transition_matrix(train))
        trans_matrix.columns = window_order
        trans_matrix.index = window_order
        
        starting_window = test.window.iloc[0]
        next_windows = []
        
        #loop through len(data) - 1 since we already have the first window
        for win in test.window[1:]:
            #if win is an index of the matrix
            if win in window_order:
                win_index = window_order.index(win)
                probs = trans_matrix.loc[win]
                next_win = probs.idxmax()
                next_windows.append(next_win)
            #if win is not in the matrix's indeces,
            #we weill predict next window to be the most common window of train data
            else:
                next_win = train.window.value_counts().idxmax()
                next_windows.append(next_win)
                
        return next_windows

    """
    Sequence HMM Predictor
    This predictor outputs the top x probable windows and 
    checks the current test data window if it's in the top x AND fits is_immersive emission matrix value
    If not then a check mark of incorrect is added 
    Accuracy is the output.
    @param  train df to train predictor on
            test df to test predictor on
    @return float accuracy 
    """
    def predictor3(self,train, test): #(self, X)

        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing)
        trans_matrix = pd.DataFrame(self.transition_matrix(train) + self.emission_matrix(train))
        trans_matrix.columns = window_order
        trans_matrix.index = window_order + ['is_immersive'] #add immersive label for the row-wise index
        
        num_correct = 0
        #Start from the very first window
        for i in range(len(test.window) - 1): # -1 to not go past last row
            #if win is an index of the matrix
            win = test.window.iloc[i]
            win_imm = test.is_immersive.iloc[i] #might be problems with nan values; havent checked it out yet
            next_win = test.window.iloc[i+1]
            next_win_imm = test.is_immersive.iloc[i]
            
            if win in window_order:
                win_index = window_order.index(win)
                probs = trans_matrix.loc[win]
                #setting it as top x, will be changed depending on accuracy
                top_prob_wins = probs.sort_values(ascending = False)[:7].index
                #check if the next window is within the threshold AND if they're immersive
                #the immersive part is redundant since its just binary values that are always related to the its respectable window
                if (next_win in top_prob_wins) and (next_win_imm == trans_matrix.loc['is_immersive', next_win]):
                    num_correct +=1
            # if window is NOT in matrix, check to see if next window is part of the top 5 most common windows
            # will also check if it is the is_immersive value of the most common value
            #ex. if train data has more 0 is_immersive than 1, then we will make 0 the "is correct" condition
            # if so, then its a correct prediction
            else:
                top_windows = train.window.value_counts(ascending = False)[:7].index
                immersive_value = train.is_immersive.value_counts().idxmax()
                if (next_win in top_windows) and (next_win_imm == immersive_value):
                    num_correct +=1
                    
        return num_correct / (len(test.window)-1)

        


