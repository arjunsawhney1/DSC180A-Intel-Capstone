# Data Collection Overview
Addressing our problem statement entails understanding the sequence in which apps are launched and how much time is spent on each app. Additional features such as app window placement may indicate the likelihood of usage. To generate this data and required features, we built input libraries in C/C++, utilizing windows APIs from Intel’s proprietary XLSDK library. Each input library extracts data alongside collection timestamps. 

## Mouse Input IL
The goal of this IL is to store and predict mouse movement data. Every 100 ms, we track the x and y position in pixel(s) of the mouse cursor and apply a 1D Kalman predictor to expose the inputs. We track the mouse noise in both the x and y positions as well as the Kalman predicted value in both dimensions.


## User Wait IL
The purpose of the user_wait_IL is to obtain the cursor type and accompanying usage times. This project required the creation of a collector thread which monitors the state of the mouse cursor icon at regular intervals (every 100ms). 

## Foreground Window IL
The purpose of the foreground_window IL is to detect and log the name of the executable whose app window sits atop all others. We implemented two waiting events to trigger the foreground window check: mouse clicks, and 1000ms time intervals. 

## Desktop Mapper IL
The Desktop Mapper input library was the most challenging to develop. Although our implementation is not optimal, we hope to eventually enrich our prediction models with features from this data. The aim is to collect detailed information on every desktop window and understand where app windows are positioned with respect to each other. The data collection is triggered whenever the foreground_window emits an iDCTL signal.
