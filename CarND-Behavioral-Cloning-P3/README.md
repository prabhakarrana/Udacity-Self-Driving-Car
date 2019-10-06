# Behaviorial Cloning Project

[![Udacity - Self-Driving Car NanoDegree](https://s3.amazonaws.com/udacity-sdc/github/shield-carnd.svg)](http://www.udacity.com/drive)

Overview
---

The goals of this project are to build a convolution neural network in Keras that predicts steering angles from images, trains and validates the model with a training and validation set and tests that the model successfully drives around track one without leaving the road.

[![Lake Track](images/lake_track.png)](https://youtu.be/bKxm2o6EMX8)
[YouTube Link](https://youtu.be/bKxm2o6EMX8)


### Dependencies
This lab requires:

* [CarND Term1 Starter Kit](https://github.com/udacity/CarND-Term1-Starter-Kit)

The lab enviroment can be created with CarND Term1 Starter Kit. Click [here](https://github.com/udacity/CarND-Term1-Starter-Kit/blob/master/README.md) for the details.

The simulator can be downloaded from the Udacity CarND classroom.

### Project Files
|  Filename   |   Description  | 
|:-------------:|:-------------:|
| prepare_data.ipynb |  ipython notebook for data preprocessing and argumentation |
| model.py | define and train the neual network |
| model.h5 | saved model by keras |
| drive.py | communicate with simulator and use saved model to predict steering angle  |
| video.mp4 | track 1 video record |

### Usage

The model can be tested by using this command:

```sh
python drive.py model.h5
```

The above command will load the trained model and use the model to make predictions on individual images in real-time and send the predicted angle back to the server via a websocket connection.

 ## Writeup is here --> [Rubric Points](https://github.com/prabhakarrana/Udacity-Self-Driving-Car/blob/master/CarND-Behavioral-Cloning-P3/writeup.md) 
