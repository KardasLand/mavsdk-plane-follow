// tracking - TrackerMain.cpp
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
#include "mavsdk.h"
#include <iostream>
#include <plugins/action/action.h>
#include "TrackerMain.h"
#include "cmath"
#include "Teknofest.h"

using namespace std;
using namespace mavsdk;

/**
 * @brief Initialize the tracker
 * @param address string IP address of the udp connection
 * @param port int Port number of the udp connection
 */
void TrackerMain::initialize(const string &address, int port = 3131) {
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_udp_connection(address, port);

    bool overrideSafety = true;

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return;
    }

    // shared pointers are kinda weird
    std::vector<std::shared_ptr<System>> systems = mavsdk.systems();
    for (const auto &system_ptr: systems) {
        auto a = &system_ptr;
        auto *adas = new plane(a->get(), system.value()->get_system_id() == system_ptr->get_system_id());
        m_planeList.push_back(adas);
    }

    plane *mainPlane = findMainPlane();
    if (mainPlane == nullptr) {
        cout << "Main plane could not be found!" << endl;
        return;
    }

    // Check the health of the plane
    // this is normally done in real life regardless of override safety
    // But we are in a simulation, so we can override the safety.
    // this is useful for testing, don't need to calibrate the plane every time
    //if (!overrideSafety) {
    //    mainPlane->checkHealth();
    //}
    //if (!mainPlane->isInAir()) {
    //    mainPlane->arm();
    //    mainPlane->takeoff();
    //}
    // little bir unnecessary but it's fine
    sleep_for(seconds(5));

    plane *targetPlane = m_planeList.at(1);
    cout << "Following...\n";


    mainPlane->startOffboard();

    cout << "Before Calling Follow\n";
    cout << mainPlane->getAltitude() << endl;
    cout << mainPlane->getLatitude() << endl;
    cout << mainPlane->getLongitude() << endl;

    //mainPlane->offGlobal(0.001,0.001,0.0,0.0);
    //Teknofest::followPlane(mainPlane, targetPlane);

    cout << "After Calling Follow\n";
    cout << mainPlane->getAltitude() << endl;
    cout << mainPlane->getLatitude() << endl;
    cout << mainPlane->getLongitude() << endl;

    mainPlane->stopOffboard();

    sleep_for(seconds(3));
    std::cout << "Finished...\n";

    // You can land the plane if you want.
    // mainPlane->land();
}

/**
 * Find the main plane
 * @return plane* the main plane
 */
plane *TrackerMain::findMainPlane() {
    for (plane *plane: m_planeList) {
        if (plane->isMainPlane()) {
            return plane;
        }
    }
    return nullptr;
}
