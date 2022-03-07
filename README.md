# Intel Intelemetry: Data Collection & Time-Series Prediction of App Usage
## Abstract
Despite advancements in hardware technology, PC users continue to face frustrating app launch times, especially on lower end Windows machines. The desktop experience differs vastly from the instantaneous app launches and optimized experience we have come to expect even from low end smartphones. We propose a solution to preemptively run Windows apps in the background based on the app usage patterns of the user. 

Our solution is two-step. First, we built telemetry collector modules in C/C++ to collect real-world app usage data from two of our personal Windows 10 devices. Next, we developed neural network models, trained on the collected data, to predict app usage times and corresponding launch sequences in python. We achieved impressive results on selected evaluation metrics across different user profiles. 

## Usage
Due to the nature of our project, we have two distinct predictive tasks.

The project pipeline for our HMM model may be run as follows:
```
launch-scipy-ml.sh
git clone git@github.com:arjunsawhney1/intel-capstone-project.git
cd intel-capstone-project/src/models/HMM
python run.py
```

The project pipeline for our LSTM model may be run as follows:
```
launch-scipy-ml.sh
git clone git@github.com:arjunsawhney1/intel-capstone-project.git
cd intel-capstone-project/src/models/LSTM
python run.py
```

## Project Website
https://arjunsawhney1.github.io/intel-capstone-project/

## Docker Image
arjunsawhney1/intel-telemetry:latest
