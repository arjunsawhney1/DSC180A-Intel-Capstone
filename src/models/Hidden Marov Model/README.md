# HMM Class Overview

## get_next_window(win, df)
Returns the list of windows that come after param win (str)

## get_cond_probs(win,df)
Returns series of the conditional prob a window appears after the given win for every other window

## transition_matrix(data)
Creates the n x n transistion matrix with n = num of unique windows. The indices and columns are the order of window_order which is just a list of unique windows (no specific order, once defined, that order persists). The rows denote the probability for window_order[win] of every other window. In other words, a number is the probability that number's col window appears after that number's row window.

## get_immersive_prob(win, data)
Returns the conditional immersive window  probability of win

## emission_matrix(data)
Returns a 1 by n emission matrix where n denotes the number of unique windows (indexed by window_order). Each value represents the probability that col window is immersive. In our case, it is just a row of 1 and 0 probabilities. 

## clean_data(window_path, imm_path)
Cleans csv and outputs either a windows dataframe OR windows and is_immersive dataframe.

## train_test_split(data,train_size, test_size)
Splits data into train and test portions, default 80-20 split 
Training data is first train_size % of the data
Test data is last test_size % of the data.

## predictor1(train, test)
This predictor outputs the top x (default x = 5) probable windows and checks the current test data window if it's in the top x.
If yes then a check mark of correct is added. Accuracy is determined by number of correct / length of data is outputted at the end.

## predictor2(train, test)
This predictor ouputs a sequence of next windows. The next window of the sequence is predicted by simply picking the window with the highest probability of appearing after the current window. Accuracy is measured AFTER by comparing the outputed sequence of windows with the test sequence of windows.

## predictor3(train, test)
This predictor outputs the top x (default x = 5) probable windows and checks the current test data window if it's in the top x AND if it's is_immersive value is equal to the is_immersive emission matrix value. If both conditions are met then a check mark of correct is added. In the case that the test window does not appear in the emission matrix, then the probability of 0 is assigned to that window's is_immersive value. Accuracy is determined by number of correct / length of data is outputted at the end.
 

