import pandas as pd
class HMM:
    def __init__(self,data):
        self.data=data

    def get_next_window(win, df):
        windows_after_win = []
        for i in range(len(df) - 1):
            if df['window'].iloc[i] == win:
                windows_after_win.append(df['window'].iloc[i+1])
        return windows_after_win
    def get_cond_probs(win,df):
        temp_windows = pd.Series(get_next_window(win, df))
        return temp_windows.value_counts() / temp_windows.value_counts().sum()
    def transition_matrix(data):
        window_order = list(data.window.unique())
        trans_matrix = []
        for curr_win in window_order:
            curr_row  = [0] * len(window_order)
            probs = get_cond_probs(curr_win, data)
            probs_dict = dict(probs)
            for win in probs.index:
                prob = probs_dict[win]
                i = window_order.index(win)
                curr_row[i] = prob
            trans_matrix.append(curr_row)
        return trans_matrix
    def get_immersive_prob(win, data):
        win_data = data[data.window == win]
        total_is_immersive = win_data.is_immersive.value_counts().sum() #this doesn't count values of NaN
        num_immersive = len(win_data[win_data.is_immersive == 1])
        return num_immersive / total_is_immersive 
    def emission_matrix(data):
        window_order = list(data.window.unique())
        emis_matrix = []
        for win in window_order:
            emis_matrix.append(get_immersive_prob(win, data))
        emis_matrix = [0.0 if math.isnan(x) else x for x in emis_matrix]
        return [emis_matrix]  
        """
    Cleans csv and outputs either a windows df OR windows and is_immersive df
    @params: data str csv path to just windows data 
            data_imm str csv path to immersive data
    @return: df
    """
    def clean_data(window_path, imm_path = 1):
        #if immersive data is not provided
        if imm_path == 1:
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
            nan_value = rows_with_NaN.iloc[0,0]
            #get rid of any number other than 0, 1 or nan value
            data = data[data.is_immersive.isin([0,1,nan_value])]
            
        return data


    """
    Splits data into train and test portions, default 80-20 split
    Training data is first train_size % of the data
    Test data is last test_size % of the data
    @params: data df to split 
            train_size float percentage of data to be training set
            test_size float percentage of data to be test set
    @return: 2 dfs (train and test dfs)
    """
    def train_test_split(data,train_size = .8, test_size = .2):
        len_train_data = train_size * len(data)
        len_test_size = test_size * len(data)
        train_data = data[:int(len_train_data)]
        test_data = data[len(data) - int(len_test_size):]
        return train_data, test_data


    #This predictor outputs the top x probable windows and checks the current test data window if it's in the top x.
    #if not then a check mark of incorrect is added 
    #accuracy is outputted at the end
    def predictor1(train, test): #(self, X)
        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing) ~ made on train data
        trans_matrix = pd.DataFrame(transition_matrix(train))
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
                top_prob_wins = probs.sort_values(ascending = False)[:5].index
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


    #This predictor ouputs a sequence of next windows. Accuracy is measured AFTER 
    def predictor2(train,test): #(self, X)
        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing)
        trans_matrix = pd.DataFrame(transition_matrix(train))
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

    #This predictor outputs the top x probable windows and 
    #checks the current test data window if it's in the top x AND fits is_immersive emission matrix value
    #if not then a check mark of incorrect is added 
    #accuracy is outputted at the end

    def predictor3(train, test): #(self, X)

        window_order = list(train.window.unique())
        #Create transition matrix (store as df for easier indexing)
        trans_matrix = pd.DataFrame(transition_matrix(train) + emission_matrix(train))
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
                #setting it as top 5, will be changed depending on accuracy
                top_prob_wins = probs.sort_values(ascending = False)[:5].index
                #check if the next window is within the threshold AND if they're immersive
                #the immersive part is redundant since its just binary values that are always related to the its respectable window
                if (next_win in top_prob_wins) and (next_win_imm == trans_matrix.loc['is_immersive', next_win]):
                    num_correct +=1
            # if window is NOT in matrix, check to see if next window is part of the top 5 most common windows
            # will also check if it is the is_immersive value of the most common value
            #ex. if train data has more 0 is_immersive than 1, then we will make 0 the "is correct" condition
            # if so, then its a correct prediction
            else:
                top_windows = train.window.value_counts(ascending = False)[:5].index
                immersive_value = train.is_immersive.value_counts().idxmax()
                if (next_win in top_windows) and (next_win_imm == immersive_value):
                    num_correct +=1
                    
        return num_correct / (len(test.window)-1)

        


