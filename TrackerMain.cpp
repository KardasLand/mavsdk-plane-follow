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
#include <plugins/offboard/offboard.h>
#include <plugins/telemetry/telemetry.h>
#include "TrackerMain.h"
using namespace std;
using namespace mavsdk;

void TrackerMain::initialize() {
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_udp_connection("localhost", 3131);

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

    // Instantiate plugins.
    auto action = Action{system.value()};
    auto offboard = Offboard{system.value()};
    auto telemetry = Telemetry{system.value()};
    //shared_ptr<System> a = *system;
    std::vector<std::shared_ptr<System>> systems = mavsdk.systems();

    for (const auto& system_ptr : systems) {
        auto a = &system_ptr;
        auto* adas = new plane(a->get(), system.value()->get_system_id() == system_ptr->get_system_id());
        m_planeList.push_back(adas);
    }

    plane* mainPlane = findMainPlane();
    if(mainPlane == nullptr){
        cout << "Main plane not found!" << endl;
        return;
    }


    if (!overrideSafety) {
        mainPlane->checkHealth();
    }
    if (!mainPlane->isInAir()){
        mainPlane->arm();
        mainPlane->takeoff();
    }

    mainPlane->takeoff();
    sleep_for(seconds(15));

    plane* targetPlane = m_planeList.at(1);
    cout << "Following...\n";
    mainPlane->startFollowing();

    //TODO this will be better later
    int i = 30;
    while(i >= 0){
        cout << "Following... " << i << endl;
        mainPlane->follow(targetPlane->getLatitude(), targetPlane->getLongitude(), (float) targetPlane->getAltitude());
        sleep_for(seconds(5));
        i--;
    }

    mainPlane->stopFollowing();

    sleep_for(seconds(3));
    std::cout << "Finished...\n";
}

/**
 * Find the main plane
 * @return plane*
 */
plane *TrackerMain::findMainPlane() {
    for(plane* plane : m_planeList){
        if(plane->isMain){
            return plane;
        }
    }
    return nullptr;
}
