[//]: # (Image References)

[code1]: ./img/1.png "Code 1"
[code2]: ./img/1.png "Code 2"
[code3]: ./img/1.png "Code 3"
[code4]: ./img/1.png "Code 4"
[code5]: ./img/1.png "Code 5"
[code6]: ./img/1.png "Code 6"


# Model Documentation
Self-Driving Car Engineer Nanodegree Program
In this project, the goal is to safely navigate around a virtual highway with other traffic that is driving +-10 MPH of the 50 MPH speed limit. The simulator provides the vehicle's localization and sensor fusion data; there is also a sparse map list of waypoints around the highway. The car should try to go as close as possible to the 50 MPH speed limit, which means passing slower traffic when possible, while other cars (from now on referred to as the Bees) may change lanes too. The car should avoid colliding with other cars at all cost as well as driving inside of the marked road lanes at all times unless going from one lane to another. The car should be able to make one complete loop around the 6946 m highway loop. Since it is trying to go 50 MPH, it should take a little over 5 minutes to complete one loop. Also, the Ego should not experience total acceleration over 10 m/s^2 and jerk that is greater than 10 m/s^3.   

# Rubic Criteria:

#### 1. The code compiles correctly.

The code complies when the cmake and make is used.

#### 2. The car is able to drive at least 4.32 miles without incident.

yes the car drives at least 4.32miles without any collision.

#### 3.The car drives according to the speed limit.

Yes the car doesn't cross the maximum speed limit i.e. 50MPH

#### 4. Max Acceleration and Jerk are not Exceeded.

Yes the car doesn't cross the max accleration and jerk limit.

#### 5. Car does not have collisions.

During by test run, the car doesn't collides any car or avoid any traffic rules.

#### 6. The car stays in its lane, except for the time between changing lanes.

During my test run the stayed on the lane apart from the lane changing time.

#### 7. The car is able to change lanes

Car changes lane when car in front of it is slower and a relative smooth lane change can take place

## Algorithm Description

1. Obtain current car telemetry, sensor fusion data, and information of the remainder of the previous path.
2. This implementation uses a spline function to fits a line to given points in a fairly smooth function for a jerk free ride. After fitting a line, then multiple data points are fed along that line (with the distance between them depending on my desired speed) back into the simulator.
3.In main.cpp to avoid the collision , we would store the car s coordinates to end_path_s value when the previous_path_size is greater than 0 so that we start from the end point for the next iteration thus avoiding any collision (line 99-102 )
![alt text][code1]

4.From sensor fusion data lane position of the other cars are determined (line 104-121) and respective flags are set (ahead,left,right) indicating other car positions using creating a boundary of 30m front and back of our car (line 129-140).
![alt text][code2]
![alt text][code3]
5. Once we are aware of other car's positions behaviour modelling is done and Code Explaination is below (line 143-173 )
![alt text][code4]   
| State  |  Action |     Description |
| ------------- | ------------- |------------- |
| Car Ahead  | Reduce Speed  | As no path available to reduce speed gradually |
| Car Ahead  | Take Left Lane | There is no car on the left and left lane is also free to turn |
| Car Ahead  | Take Right Lane | There is no car on the right and right lane is also free to turn |
| Free Lane Ahead | Increase Speed  |Increase speed till Max Speed limit as road is free|
| Free Lane Ahead | Move back to Middle Lane  |If middle lane is free move to middle lane|

7.The following code snippet shows how we use the spine library to get the next wavepoints(line 214-216 ).
![alt text][code5]
8.Pushed these points into a vector (line 219-225).
![alt text][code6]
9.Filled intermediate paths points using spline and ref_velocity is also increased /decreased as per state conditions with a step of 0.224 line(255-263).


