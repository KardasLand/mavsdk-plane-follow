# Mavsdk Plane Follow

[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)

This application uses Mavsdk to follow other planes and do some other tasks such as getting telemetry data,
landing/taking off, arming and offsetting the plane based on global coordinates.

My goal of this application is slowly transforming the application from executing simple commands to full fledged simple
library to make lives of people who just started tinkering with mavsdk a little bit easier.

Keep in mind that as you can see, we need 2 planes to test this application.
One of them will be the leader and the other one will be the follower.
The follower will follow the leader based on the leader's global coordinates.
Because of this, to simulate the 2 plane, we are using MavProxy with PX4-Autopilot and Gazebo Classic.

[I will be explaining on my personal blog later, but idk when.](https://anilsayar.com)

## Libraries

- Mavsdk
- C++ 11 Libraries such as future, chrono and thread.
- MavProxy for simulation

## Authors

- [@kardasland](https://www.anilsayar.com)

