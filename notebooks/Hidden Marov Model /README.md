## HMM Class Overview

# get_next_window(win, df)
Returns the list of windows that come after param win (str)
# get_cond_probs(win,df)
Returns series of conditional probs for succeeding window(s) for a given win
# transition_matrix(data)
Returns transisiton matrix for the HMM and creates the n x n transistion matrix with n = num of unique windows indices is the order of window_order
rows denote prob for window_order[win] for every other window
# get_immersive_prob(win, data)
Returns the conditional immersive window  probabilities 
# emission_matrix(data)
Returns emission matrix using only emmersive probabilty
# clean_data(window_path, imm_path)
Cleans csv and outputs either a windows df OR windows and is_immersive df
# train_test_split(data,train_size, test_size)
Splits data into train and test portions, default 80-20 split 
Training data is first train_size % of the data
Test data is last test_size % of the data
# predictor1(train, test)
This predictor outputs the top x probable windows and checks the current test data window if it's in the top x.
if not then a check mark of incorrect is added. Accuracy is outputted at the end
# predictor2(train,test)
This predictor ouputs a sequence of next windows. Accuracy is measured AFTER 
# predictor3(train, test)
This predictor outputs the top x probable windows and 
checks the current test data window if it's in the top x AND fits is_immersive emission matrix value
if not then a check mark of incorrect is added 
accuracy is outputted at the end
 

