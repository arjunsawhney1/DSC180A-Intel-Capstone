# Intel Systems Usage Reporting
## Data Science Capstone Project
- 20 week collaborative capstone project advised by Intel leadership
- 10 weeks developing Intel® Energy Checker Energy Server data collection modules
- 10 weeks creating various ML models (LSTM RNN, Hidden Markov Chains) for collected time-series
data to predict and pre-emptively launch apps

## Launching the DLL and collecting Data
Run `launch.bat` to start collecting data using the foreground_window and user_wait dynamic load libraries.

If the software is working correctly, you should notice the number of samples incrementing at a rate of 1Hz.

## Testing
Run `test.sh` to check the data outputs for correctness. This script summarizes the data and checks that values make sense.
