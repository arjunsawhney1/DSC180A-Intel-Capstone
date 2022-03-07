# Prediction Overview
We came up with 2 prediction problems to better understand app usage behaviors, using 3 weeks of our collected time-series window data starting from January 3rd 2022. 


## Prediction Task 1: HMM
The first problem is to identify patterns in app launches. 
The HMM is best suited for our first problem as it allows us to understand the progression of events based on conditional probabilities. It works on the assumption that the future state relies on the current state which is important considering we are using sequential window data. We implemented three predictors using different heuristics for measuring accuracy to see how they compared with each other.

## Prediction Task 2: LSTM
The second problem is to predict app usage durations and corresponding app sequences. 
The LSTM is appropriate for our second solution since it’s typically used for forecasting problems without being affected by the vanishing gradient problem where convergence happens too quickly. For the second prediction task, we attempted two approaches to the problem: a univariate regression and a multivariate classification.
