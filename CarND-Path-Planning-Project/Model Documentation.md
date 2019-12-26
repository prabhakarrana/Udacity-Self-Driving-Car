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
3.In main.cpp to avoid the collision , we would store the car s coordinates to end_path_s value when the previous_path_size is greater than 0 so that we start from the end point for the next iteration thus avoiding any collision (line  )
4.From sensor fusion data lane position of the other cars are determined (line ) and respective flags are set (ahead,left,right) indicating other car positions using creating a boundary of 30m front and back of our car (line ).
5. Once we are aware of other car's positions behaviour modelling is done and high-level diagram is shown below.
6. Code Explaination (line )
   
| First Header  | Second Header |
| ------------- | ------------- |
| Content Cell  | Content Cell  |
| Content Cell  | Content Cell  |


remaining path is cross-checked.If it is the first time around, then I have to initialized my points w.r.t to car's current position so that it can be used in spline function. If there are remaining path, they will get pushed back into the x and y coordinates to be fed the simulator again and calculated the x, y, yaw and car velocity .

4.Changed  x & y coordinates into frenet coordinates, s & d (line 298). In frenet, "s" is the distance along a lane while "d" is the distance away from the center dividing line of the road (at least in the simulator). 

Now it's time for the behavior planner itself. In behavior.cpp, I start by calculating which lane I am in based off of "d" - "0" for the left lane in my direction, "1" for the middle lane, and "2" for the right lane. I then check how close the car in front of me is with ClosestVehicle(), which can be set to look either in front of or behind my vehicle's "s" position in different lanes. In this case, I first am just checking in front of my car - if the closest vehicle in my lane is more than 20 meters away, I want to drive near the speed limit and hold my lane.

The interesting part comes if my car does have another car close in front of it. This is where laneScore() comes in. This iterates through all three lanes, and checks both in front of and behind my car for the closest vehicles. I added a slight benefit to holding my lane, with higher negative values if there are cars too close in front or behind my vehicle, as I would want to avoid changing lanes at that point (or staying in my own lane) because an accident would likely occur. There are then benefits (or costs) depending on how much distance there is in front and behind the vehicles in the given lane and what speed those vehicles are going.

These scores are then taken as an "average" of the last ten scores for that lane (not exactly, but in a simplified manner), and then the best scoring lane is returned from the function. Back into lanePlanner(), I then make sure to keep the vehicle's speed in front of me in the lane I want to move to (or keep) so that I can match it. Finally, lanePlanner() returns a move in meters - "0" if keeping the same lane, "-4" if changing lanes left, and "4" if changing lanes right.

This brings it back into main.cpp, where the next "d" value gets calculated (line 302) to be used in setting waypoints for the spline function. However, there is still one last check performed to make sure that the lane desired is actually going to be open for our move (lines 304-315) - if it isn't, then the lane gets reset to the current one, and the target speed is reset to the old vehicle being followed.

Finally, it's time to make waypoints for the spline function. Based off the new desired "d", I equally space out the waypoints by fifty meters - too much shorter sometimes caused it to exceed max acceleration/jerk. These, along with any from the old path (or the car starting point if initializing) are then shifted and rotated so that they are local to my own vehicle (lines 330-338). This helps to ensure the spline can work correctly, as the x-values need to be in order, and we want to the be correctly spaced out going forward.

After setting the spline, I use the spline to come up with new points along that spline. For each point, I space them out over the target distance using the current (per the calc) velocity over 0.02 seconds, as the simulator moves every 20 milliseconds. For each point, I compare the velocity (ref_vel) to my target vehicle's speed, and either accelerate or deccelerate based on where I am in comparison. The "x" value is then changed by the distance traveled at that speed, and "y" is calculated off the spline value for that "x". These points are then rotated and shifted back to global coordinates, and fed to the simulator to drive!
