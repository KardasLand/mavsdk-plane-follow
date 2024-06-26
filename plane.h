#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
// tracking - plane.h
// Copyright (c) 2024 Neo Stellar Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Created by kardasland on 3/24/24.
//

#ifndef TRACKING_PLANE_H
#define TRACKING_PLANE_H

#include <chrono>
#include <thread>
#include <mavsdk/plugins/follow_me/follow_me.h>
#include "mavsdk.h"
#include "mavsdk/plugins/action/action.h"
#include "mavsdk/plugins/offboard/offboard.h"
#include "mavsdk/plugins/telemetry/telemetry.h"

using namespace mavsdk;
using namespace std;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;

class plane {
public:
    plane(System *sharedPtr, bool isMain);

    bool offGlobal(double latOff, double longOff, double altOff, double yawOff) const;
    bool startOffboard();
    bool stopOffboard();

    void checkHealth() const;

    bool arm() const;

    bool takeoff();

    bool land() const;

    bool startFollowing();

    void follow(double lat, double lon, float alt) const;

    bool stopFollowing() const;

    bool isInAir() const;

    bool hasCamera(int camera_id) const;

    bool setCameraMode(Camera::Mode mode);

    bool takePhoto() const;

    bool startVideo(bool stream, int streamID) const;

    bool stopVideo(bool stream, int streamID) const;

    void debug(bool detailed) const;

    double getLatitude() const {
        return latitude;
    };

    double getAltitude() const {
        return altitude;
    };

    double getAirSpeed() const;

    double getLongitude() const {
        return longitude;
    };

    bool isMainPlane() const {
        return isMain;
    };

    const Telemetry &getTelemetry() const {
        return telemetry;
    }

    System *system;
    Telemetry telemetry{*system};
    Action action{*system};
    Offboard offboard{*system};
    FollowMe followMe{*system};
    Camera camera{*system};

private:
    int sysid{};
    [[maybe_unused]] int teamID{};
    int yaw{};
    double latitude{};
    double altitude{};
    double longitude{};
    void init();
    bool isMain;
};


#endif //TRACKING_PLANE_H

#pragma clang diagnostic pop