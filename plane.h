//
// Created by kardasland on 3/24/24.
//

#ifndef TRACKING_PLANE_H
#define TRACKING_PLANE_H

#include <chrono>
#include <thread>
#include <mavsdk/plugins/follow_me/follow_me.h>
#include "mavsdk.h"

using namespace mavsdk;
using namespace std;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;
class plane {
public:
    plane(System *sharedPtr, bool isMain);
    bool offGlobal(double latOff, double longOff, double altOff, double yawOff);
    void checkHealth();
    int arm();
    int takeoff();
    int land();
    int startFollowing();
    int follow(double lat, double lon, float alt);
    int stopFollowing();
    bool isInAir();
    bool isMain;
    System *system;
    Telemetry telemetry{*system};
    Action action{*system};
    Offboard offboard{*system};
    FollowMe followMe{*system};
private:
    int sysid;
    int teamid;
    int yaw;
    double latitude;
    double altitude;
    double longitude;
    void init();
public:
    int getSysid() const;

    void setSysid(int sysid);

    int getTeamid() const;

    void setTeamid(int teamid);

    int getYaw() const;

    void setYaw(int yaw);

    double getLatitude() const;

    void setLatitude(double latitude);

    double getAltitude() const;

    void setAltitude(double altitude);

    double getLongitude() const;

    void setLongitude(double longitude);

    void debug() const;
};


#endif //TRACKING_PLANE_H
