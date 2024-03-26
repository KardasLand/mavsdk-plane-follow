// tracking - main.cpp
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

#include "TrackerMain.h"

int main() {

    /* TODO
     *  Needing to fix & implement the following functionalities:
     *  - Follow me functionality
     *  - Offboard control
     *  - Inverse the int functions to return a bool
     */

    TrackerMain trackerMain;
    // Initialize the tracker
    int port = 3131;
    string ip = "localhost";
    trackerMain.initialize(ip, port);
    return 0;
}
