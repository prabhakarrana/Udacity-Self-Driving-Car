#include <uWS/uWS.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "helpers.h"
#include "json.hpp"
#include "spline.h"

// for convenience
using nlohmann::json;
using std::string;
using std::vector;

int main() {
uWS::Hub h;

// Load up map values for waypoint's x,y,s and d normalized normal vectors
vector<double> map_waypoints_x;
vector<double> map_waypoints_y;
vector<double> map_waypoints_s;
vector<double> map_waypoints_dx;
vector<double> map_waypoints_dy;

// Waypoint map to read from
string map_file_ = "../data/highway_map.csv";
// The max s value before wrapping around the track back to 0
double max_s = 6945.554;

std::ifstream in_map_(map_file_.c_str(), std::ifstream::in);

string line;
while (getline(in_map_, line)) {
  std::istringstream iss(line);
  double x;
  double y;
  float s;
  float d_x;
  float d_y;
  iss >> x;
  iss >> y;
  iss >> s;
  iss >> d_x;
  iss >> d_y;
  map_waypoints_x.push_back(x);
  map_waypoints_y.push_back(y);
  map_waypoints_s.push_back(s);
  map_waypoints_dx.push_back(d_x);
  map_waypoints_dy.push_back(d_y);
}
  // Car lane. Currently three lane id 0,1,2
  int lane = 1;
  // Reference velocity.
  double ref_vel = 0.0;
  h.onMessage([&ref_vel, &lane, &map_waypoints_x,&map_waypoints_y,&map_waypoints_s,&map_waypoints_dx,&map_waypoints_dy]
  (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,uWS::OpCode opCode) {
  // "42" at the start of the message means there's a websocket message event.
  // The 4 signifies a websocket message
  // The 2 signifies a websocket event
  if (length && length > 2 && data[0] == '4' && data[1] == '2') {

    auto s = hasData(data);

    if (s != "") {
      auto j = json::parse(s);

      string event = j[0].get<string>();

      if (event == "telemetry") {
        // j[1] is the data JSON object

        // Main car's localization Data
        double car_x = j[1]["x"];
        double car_y = j[1]["y"];
        double car_s = j[1]["s"];
        double car_d = j[1]["d"];
        double car_yaw = j[1]["yaw"];
        double car_speed = j[1]["speed"];

        // Previous path data given to the Planner
        auto previous_path_x = j[1]["previous_path_x"];
        auto previous_path_y = j[1]["previous_path_y"];
        // Previous path's end s and d values 
        double end_path_s = j[1]["end_path_s"];
        double end_path_d = j[1]["end_path_d"];

        // Sensor Fusion Data, a list of all other cars on the same side 
        //   of the road.
        auto sensor_fusion = j[1]["sensor_fusion"];

        // Provided previous path point size.
        int prev_size = previous_path_x.size();

          json msgJson;

          // ######### Other cars statistics Position and velocity #######
          // Prevents collision by setting obstruction flags e.g too_close,car_ahead,car_left,car_right.
            if (prev_size > 0) {
                car_s = end_path_s;
            }

          bool too_close = false;
          bool car_ahead = false;
          bool car_left = false;
          bool car_right = false;
          for ( int i = 0; i < sensor_fusion.size(); i++ ) {
              float d = sensor_fusion[i][6];
              int car_lane = -1;
              // find lane of other car
              if ( d > 0 && d < 4 ) {
                car_lane = 0;  //first lane
              } else if ( d > 4 && d < 8 ) {
                car_lane = 1;  //second lane
              } else if ( d > 8 && d < 12 ) {
                car_lane = 2;  //third lane
              }
              if (car_lane < 0) {
                continue; // invalid lane
              }
              // Other cars speed.
              double vx = sensor_fusion[i][3];
              double vy = sensor_fusion[i][4];
              double check_speed = sqrt(vx*vx + vy*vy);
              double check_car_s = sensor_fusion[i][5];
            
              // Estimate other car s coordinate.
              check_car_s += ((double)prev_size*0.02*check_speed);
              if ( car_lane == lane ) {
                   // Car in our lane.
                    car_ahead |= check_car_s > car_s && check_car_s - car_s < 30;
                } else if ( car_lane - lane == -1 ) {
                       // Car left
                    car_left|= car_s - 30 < check_car_s && car_s + 30 > check_car_s;
                } else if ( car_lane - lane == 1 ) {
                     // Car right
                    car_right |= car_s - 30 < check_car_s && car_s + 30 > check_car_s;
                }
            }
          // Velocity and lane control in case of obstruction

          double auto_speed = 0.0;       	  
          const double max_speed = 49.5;
          const double max_acc = 0.224;
          if ( car_ahead ) { // Car ahead

            if ( !car_left && lane > 0 ) {
              // no car on left and left lane free.
              lane-=1; // Change lane left. 
               //std::cout<<"no car on left and left lane free:"<<lane<<"\n";            
              
            } else if ( !car_right && lane != 2 ){
              //  no car on right and right lane free.
              lane+=1; // Change lane right.             
              //std::cout<<"no car on right and right lane free:"<<lane<<"\n";
              
            } else {
              //std::cout<<"Decreasing speed as no path available\n";
               //speed_diff -= 0.224;
              auto_speed -= max_acc; 
            }
          } else { // Free lane ahead
            if ( lane != 1 ) { // not the middle lane.
              if ( ( lane == 0 && !car_right ) || ( lane == 2 && !car_left ) ) {
                 lane = 1; // Move to middle lane againr.
              }
            }
            //increase car speed
            if ( ref_vel < max_speed ) {
               auto_speed += max_acc;
            }
          }

          vector<double> ptsx;
          vector<double> ptsy;

          double ref_x_t2 = car_x;
          double ref_y_t2 = car_y;
          double ref_yaw = deg2rad(car_yaw);

          // validate for previous points, for less than 2 points use starting reference
          if ( prev_size < 2 ) {
              // create two points to form tangent path according to x and y axis
              double prev_x = car_x - cos(car_yaw);
              double prev_y = car_y - sin(car_yaw);

              ptsx.push_back(prev_x);
              ptsx.push_back(car_x);

              ptsy.push_back(prev_y);
              ptsy.push_back(car_y); 
              // Use previous two points as starting reference.
          } else {

              // create two points to form tangent path for time t and t-1
              ref_x_t2 = previous_path_x[prev_size - 1];
              ref_y_t2 = previous_path_y[prev_size - 1];

              double ref_x_t1 = previous_path_x[prev_size - 2];
              double ref_y_t1 = previous_path_y[prev_size - 2];
              ref_yaw = atan2(ref_y_t2-ref_y_t1, ref_x_t2-ref_x_t1);

              ptsx.push_back(ref_x_t1);
              ptsx.push_back(ref_x_t2);

              ptsy.push_back(ref_y_t1);
              ptsy.push_back(ref_y_t2);
          }

          // Configure three target points at 30,60,90 for future and then random points will be selected inside these three points 
          // to make jerkfree driving in Frenet coordinates.

          vector<double> next_sp0 = getXY(car_s + 30, 2 + 4*lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);
          vector<double> next_sp1 = getXY(car_s + 60, 2 + 4*lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);
          vector<double> next_sp2 = getXY(car_s + 90, 2 + 4*lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);

          //x-coordinates
          ptsx.push_back(next_sp0[0]);
          ptsx.push_back(next_sp1[0]);
          ptsx.push_back(next_sp2[0]);
          //y-coordinates
          ptsy.push_back(next_sp0[1]);
          ptsy.push_back(next_sp1[1]);
          ptsy.push_back(next_sp2[1]);

          // Making coordinates to local car coordinates.
          for ( int i = 0; i < ptsx.size(); i++ ) {
            double shift_x = ptsx[i] - ref_x_t2;
            double shift_y = ptsy[i] - ref_y_t2;

            ptsx[i] = shift_x * cos(0 - ref_yaw) - shift_y * sin(0 - ref_yaw);
            ptsy[i] = shift_x * sin(0 - ref_yaw) + shift_y * cos(0 - ref_yaw);
          }

          // Create the spline.
          tk::spline s;
          s.set_points(ptsx, ptsy);

          // Output path points from previous path for continuity.
          vector<double> next_x_vals;
          vector<double> next_y_vals;
          for ( int i = 0; i < prev_size; i++ ) {
            next_x_vals.push_back(previous_path_x[i]);
            next_y_vals.push_back(previous_path_y[i]);
          }

          // Calculate distance y position on 30 m ahead.
          double target_x = 30.0;
          double target_y = s(target_x);
          double target_dist = sqrt(target_x*target_x + target_y*target_y);

          double x_add_on = 0;

          for( int i = 1; i < 50 - prev_size; i++ ) {
            ref_vel += auto_speed;
            //std::cout<<"ref_vel:"<<ref_vel<<"\n";
            //std::cout<<"speed_diff:"<<speed_diff;      
              if ( ref_vel > max_speed ) {
                ref_vel = max_speed;
              } else if ( ref_vel < max_acc  ) {
                ref_vel = max_acc ;
              }
            double N = target_dist/(0.02*ref_vel/2.24);
            double x_point = x_add_on + target_x/N;
            double y_point = s(x_point);

            x_add_on = x_point;

            double x_ref = x_point;
            double y_ref = y_point;

            x_point = x_ref * cos(ref_yaw) - y_ref * sin(ref_yaw);
            y_point = x_ref * sin(ref_yaw) + y_ref * cos(ref_yaw);

            x_point += ref_x_t2;
            y_point += ref_y_t2;

            next_x_vals.push_back(x_point);
            next_y_vals.push_back(y_point);
          }


        msgJson["next_x"] = next_x_vals;
        msgJson["next_y"] = next_y_vals;

        auto msg = "42[\"control\","+ msgJson.dump()+"]";

        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }  // end "telemetry" if
    } else {
      // Manual driving
      std::string msg = "42[\"manual\",{}]";
      ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
    }
  }  // end websocket if
}); // end h.onMessage

h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
  std::cout << "Connected!!!" << std::endl;
});

h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
                       char *message, size_t length) {
  ws.close();
  std::cout << "Disconnected" << std::endl;
});

int port = 4567;
if (h.listen(port)) {
  std::cout << "Listening to port " << port << std::endl;
} else {
  std::cerr << "Failed to listen to port" << std::endl;
  return -1;
}

h.run();
}