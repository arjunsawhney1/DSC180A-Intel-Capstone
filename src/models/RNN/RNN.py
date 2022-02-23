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
    Extract time (timedelta64[ns] dtype) spent within each app.
    Last window will have 0 time spent on it.
    @param  df cleaned data
    @return series with time spent on window 
    """
    def calc_time(self, data):
        #find difference of timestamp of next window - timestamp of current window
        data['tvalue'] = data.time
        data['delta'] = (data['tvalue'] - data['tvalue'].shift()).fillna(pd.Timedelta(seconds=0))

        #Shift rows up (first row was 0) add a 0 DT value at end
        time_col = list(data['delta'][1:]) + [pd.Timedelta(seconds=0)]
        time_spent = pd.Series(time_col)
        time_spent.index = data.window

        return time_spent

    """
    Loads data into a df; drop + rename cols 
    Changes time to DateTime obj
    Drops back-to-back windows
    Adds column "time_spent" (time spent on an app) for each window
    @param  str csv window path
    @return df cleaned data
    """
    def clean_data(self, path):
        df = pd.read_csv(path)
        #drop unecessary cols and rename the remaining cols
        df = df.drop(['ID_INPUT','PRIVATE_DATA'], axis = 1)
        df.columns = ['time','window']

        #change time col to datetime object
        df['time'] = pd.to_datetime(df['time'])

        #handle back-to-back same windows
        clean_df = self.drop_dups(df)

        #Add column of time spent on each app
        clean_df['time_spent'] =  list(self.calc_time(clean_df))

        #Drop calculation columns from calc_time
        clean_df = clean_df.drop(['tvalue','delta'], axis=1)

        return clean_df

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
    Converts timedelta obj to minutes
    @param col series of timedelta obj
    @return list of float minutes
    """
    def to_minutes(self, col):
        minutes = []
        for time in col:
            minutes.append(time.seconds / 60)
        return minutes

    """
    Converts timedelta obj to seconds
    @param col series of timedelta obj
    @return list of int seconds
    """
    def to_seconds(self, col):
        seconds = []
        for time in col:
            seconds.append(time.seconds)
        return seconds

    """
    Compares predicted and test time_spent values using a threshold 
    Outputs an accuracy rating
    @param preds container of predicted time_spent values
           test container of test time_spent values
           threshold int number of allowed +- seconds to be off 
                    default is 1 second off
    @return float accuracy rating
    """
    def test_accuracy(self, preds, test, threshold=1):
        num_correct = 0
        for i in range(len(preds)):
            pred_time = preds[i]
            test_time = test[i]
            # checks to see if predicted time_spent is between 
            # actual time spent +- threshold
            if (pred_time >= test_time-threshold) & (
                pred_time <= test_time+threshold): 
                num_correct += 1
        return num_correct / len(preds)