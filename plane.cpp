// tracking - plane.cpp
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

#include <plugins/action/action.h>
#include <plugins/offboard/offboard.h>
#include <plugins/telemetry/telemetry.h>
#include <future>
#include "plane.h"
#include "iostream"

using namespace std;


/**
 * Constructor for the plane object
 * @param sharedPtr System pointer
 * @param isMain bool to check if the plane is the main plane
 */
plane::plane(System *sharedPtr, bool isMain)
        : isMain(isMain), system(sharedPtr) {
    init();
}

/**
 * Debug function to print the plane information
 * @return void
 */
void plane::debug() const {
    cout << "Plane ID " << sysid << " has been loaded!" << endl;
    cout << "Information: " << endl;
    cout << "Latitude: " << latitude << " Longitude: " << longitude << " Altitude: " << altitude << endl;
    cout << "Main Plane: " << (isMain ? "true" : "false") << endl;
}

/**
 * Initialize the plane object
 * This function is called in the constructor
 * It sets the system id, latitude, longitude and altitude
 * It also subscribes to the position of the plane
 * @return void
 */
void plane::init() {
    sysid = system->get_system_id();
    Telemetry::GpsGlobalOrigin origin = telemetry.get_gps_global_origin().second;
    latitude = origin.latitude_deg;
    longitude = origin.longitude_deg;
    altitude = origin.altitude_m;
     telemetry.subscribe_position([this](Telemetry::Position position) {
        latitude = position.latitude_deg;
        longitude = position.longitude_deg;
        altitude = position.absolute_altitude_m;
        //cout << "Position updated: " << latitude << " " << longitude << " " << altitude << endl;
    });

    debug();
}

/**
 * Checks and awaits for the system to be ready
 * @return void
 */
void plane::checkHealth() const {
    while (!telemetry.health_all_ok()) {
        cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }
    cout << "System is ready\n";
}

/**
 * Takeoff the plane
 * @return true if success
 * @return false if failed
 */
bool plane::takeoff() {
    const auto takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        cerr << "Takeoff failed: " << takeoff_result << '\n';
        return false;
    }
    auto in_air_promise = promise<void>{};
    auto in_air_future = in_air_promise.get_future();
    Telemetry::LandedStateHandle handle = telemetry.subscribe_landed_state(
            [this, &in_air_promise, &handle](Telemetry::LandedState state) {
                if (state == Telemetry::LandedState::InAir) {
                    cout << "Taking off has finished\n.";
                    telemetry.unsubscribe_landed_state(handle);
                    in_air_promise.set_value();
                }
            });
    in_air_future.wait_for(seconds(10));
    if (in_air_future.wait_for(seconds(3)) == future_status::timeout) {
        cerr << "Takeoff timed out.\n";
        return false;
    }
    return true;
}

/**
 * Arm the plane
 * @return true if success
 * @return false if failed
 */
bool plane::arm() const {
    const auto arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        cerr << "Arming failed: " << arm_result << '\n';
        return false;
    }
    cout << "Armed\n";
    return true;
}

/**
 * Offset the plane in global coordinates
 * @warning Parameters are only offset, not the actual coordinates.
 * @warning Do note that when you want to get increase altitude, you have to use negative values.
 * @param latOff latitude offset
 * @param longOff  longitude offset
 * @param altOff altitude offset
 * @param yawOff yaw offset
 * @return true if success
 * @return false if failed
 */
bool plane::offGlobal(double latOff, double longOff, double altOff, double yawOff) const {

    const auto res_and_gps_origin = telemetry.get_gps_global_origin();
    if (res_and_gps_origin.first != Telemetry::Result::Success) {
        cerr << "Telemetry failed: " << res_and_gps_origin.first << '\n';
    }

    cout << "Reading home position in Global coordinates\n";
    cout << "Origin (lat, lon, alt amsl):\n " << res_and_gps_origin.second << '\n';

    Telemetry::GpsGlobalOrigin origin = res_and_gps_origin.second;
    cout << "Applying offset\n";
    const Offboard::PositionGlobalYaw positionGlobalYaw{
            origin.latitude_deg + latOff,
            origin.longitude_deg + longOff,
            static_cast<float>(altitude + altOff),
            static_cast<float>(yawOff),
            Offboard::PositionGlobalYaw::AltitudeType::RelHome};
    offboard.set_position_global(positionGlobalYaw);

    return true;
}
/**
 * Land the plane
 * @return true if success
 * @return false if failed
 */
bool plane::land() const {
    const auto land_result = action.land();
    if (land_result != Action::Result::Success) {
        cerr << "Landing failed: " << land_result << '\n';
        return false;
    }
    // Check if vehicle is still in air
    while (telemetry.in_air()) {
        cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    cout << "Landed!\n";
    return true;
}

/**
 * Check if the plane is in air
 * @return true if in air
 */
bool plane::isInAir() const {
    return telemetry.in_air();
}

/**
 * Start following the plane
 * @return true if success
 * @return false if failed
 */
bool plane::startFollowing() {
    cout << "Starting to follow...\n";
    telemetry.subscribe_flight_mode([&](Telemetry::FlightMode flight_mode) {
        const FollowMe::TargetLocation last_location = followMe.get_last_location();
        cout << "[FlightMode: " << flight_mode
             << "] Target is at: " << last_location.latitude_deg << ", "
             << last_location.longitude_deg << " degrees.\n";
    });
    FollowMe::Config config;
    config.follow_height_m = 12.f;  // Minimum height
    config.follow_angle_deg = 180.f;  // Follow from behind
    FollowMe::Result config_result = followMe.set_config(config);
    if (config_result != FollowMe::Result::Success) {
        // handle config-setting failure (in this case print error)
        cout << "Setting configuration failed:" << config_result << '\n';
        return false;
    }

    cout << "Config Set\n";

    FollowMe::Result follow_me_result = followMe.start();
    if (follow_me_result != FollowMe::Result::Success) {
        // handle start failure (in this case print error)
        cout << "Failed to start following:" << follow_me_result << '\n';
        return false;
    }

    cout << "Follow me started\n";
    return true;
}

/**
 * Follow the plane with the given coordinates
 * @param lat latitude
 * @param lon longitude
 * @param alt altitude
 */
void plane::follow(double lat, double lon, float alt = 0.0f) const {
    //auto follow_me = FollowMe{*system};
    cout << "Main plane location: " << latitude << " " << longitude << " " << altitude << "\n";
    cout << "Target plane location: " << lat << " " << lon << " " << alt << "\n";
    FollowMe::TargetLocation location;
    location.latitude_deg = lat;
    location.longitude_deg = lon;
    followMe.set_target_location(location);
}

/**
 * Stop following the plane
 * @return true if success
 * @return false if failed
 */
bool plane::stopFollowing() const {
    //auto follow_me = FollowMe{*system};
    // Stop following
    FollowMe::Result follow_me_result = followMe.stop();
    if (follow_me_result != FollowMe::Result::Success) {
        // handle stop failure (in this case print error)
        cout << "Failed to stop following:" << follow_me_result << '\n';
        return false;
    }
    return true;
}

bool plane::startOffboard() {
    cout << "Reading home position in Global coordinates\n";

    const auto res_and_gps_origin = telemetry.get_gps_global_origin();
    if (res_and_gps_origin.first != Telemetry::Result::Success) {
        cerr << "Telemetry failed: " << res_and_gps_origin.first << '\n';
    }

    Telemetry::GpsGlobalOrigin origin = res_and_gps_origin.second;
    cerr << "Origin (lat, lon, alt amsl):\n " << origin << '\n';

    cout << "Starting Offboard position control in Global coordinates\n";

    // Send it once before starting offboard, otherwise it will be rejected.
    // this is a step north about 10m, using the default altitude type (altitude relative to home)
    const Offboard::PositionGlobalYaw north{
            origin.latitude_deg + 0.0001, origin.longitude_deg, 90.0f, 0.0f};
    offboard.set_position_global(north);

    Offboard::Result offboard_result = offboard.start();
    if (offboard_result != Offboard::Result::Success) {
        cerr << "Offboard start failed: " << offboard_result << '\n';
        return false;
    }

    cout << "Offboard started\n";
    return true;
}

/**
 * Stop offboard mode
 * @return true if success
 * @return false if failed
 */
bool plane::stopOffboard() {
    Offboard::Result offboard_result = offboard.stop();
    if (offboard_result != Offboard::Result::Success) {
        cerr << "Offboard stop failed: " << offboard_result << '\n';
        return false;
    }
    cout << "Offboard stopped\n";
    return true;
}

/**
 * Get the airspeed of the plane
 * @return airspeed
 */
double plane::getAirSpeed() const {
    Telemetry::VelocityNed velocity = telemetry.velocity_ned();
    return sqrt(velocity.east_m_s * velocity.east_m_s + velocity.north_m_s * velocity.north_m_s);
}



