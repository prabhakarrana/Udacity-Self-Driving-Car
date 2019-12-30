[//]: # (Image References)

[code1]: ./IMG/1.PNG "Code 1"

# Model Documentation
## PID Controller

Given a driving simulator which provides the cross track error (CTE) and the velocity (mph), the goal of this project is to develop a PID controller in c++ that successfully drives the vehicle around the track. FOr this i have implemeneted 2 PID controllers, one for steering and other for speed.

## Tuning
The most important part of the project is to tune the hyperparameters. 

#### Proportional (P): 
This parameter controls the error proportionally. Increasing the proportional gain has the effect of proportionally increasing the control signal for the same level of error. Setting only P control is agressive and has oscillations.

#### Integral (I): 
This parameter controls the accumulating error. Addition of this term reduces the steady state error. If there is a bias in the system, the integrator builds and builds, thereby increasing the control signal and driving the error down. 

#### Derivative (D): 
This parameter controls the rate of change of error. Addition of this term reduces the oscillary effect in  the system. With derivative control, the control signal can become large if the error begins sloping upward, even while the magnitude of the error is still relatively small. This anticipation tends to add damping to the system, thereby decreasing overshoot.

Final parameters for my controllers are (line 42-43): 
  
|   | Steering  | Speed  |
|---|---|---|
| Kp  |  0.13 |  0.1 |
| Ki  | 0.00  |  0.002 |
| Kd  | 1.0  |  0.0 |

With above mentioned parameters i was able to drive my car within lane and with a maximum speed of 49mph.
