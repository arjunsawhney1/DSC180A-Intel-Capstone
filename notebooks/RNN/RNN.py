import pandas as pd
import math

class RNN:
    """
    Init Function; temp value of an empty list
    @param  
    @return 
    """
    def __init__(self):
        self.temp = []

    """
    Helper function to drop back-to-back windows of a dataframe
    Called only in clean_data
    @param  df semi-clean data
    @return df with no duplicate windows
    """
    def drop_dups(self, df):
        bool_indexer = [True] * len(df)
        for i in range(len(df) - 1): #-1 to avoid IndexOutOfBoundsError
            curr_win = df.iloc[i].window
            next_win = df.iloc[i+1].window
            #if the next window is the same then add index to indeces to drop list
            if curr_win == next_win:
                bool_indexer[i+1] = False
        return df[bool_indexer]

    """
    Loads data into a df; drop + rename cols 
    Changes time to DateTime obj
    Drops back-to-back windows
    @param  str csv window path
    @return df cleaned data
    """
    def clean_data(self, path):
        df = pd.read_csv(path)
        #if path refers to a window csv
        if df.VALUE.dtype == 'object':
            #drop unecessary cols and rename the remaining cols
            df = df.drop(['ID_INPUT','PRIVATE_DATA'], axis = 1)
            df.columns = ['time','window']

            #change time col to datetime object
            df['time'] = pd.to_datetime(df['time'])

            #handle back-to-back same windows
            clean_df = self.drop_dups(df)
        return clean_df

    """
    Extract time (timedelta64[ns] dtype) spent within each app.
    Last window will have 0 time spent on it.
    @param  df cleaned data
    @return series with time spent on window 
    """
    def calc_time(self, data):
        
        data['tvalue'] = data.time
        data['delta'] = (data['tvalue'] - data['tvalue'].shift()).fillna(pd.Timedelta(seconds=0))

        #Shift rows up (first row was 0) add a 0 DT value at end
        time_col = list(data['delta'][1:]) + [pd.Timedelta(seconds=0)]
        time_spent = pd.Series(time_col)
        time_spent.index = data.window
        
        return time_spent