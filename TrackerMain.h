// tracking - TrackerMain.h
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

#ifndef TRACKING_TRACKERMAIN_H
#define TRACKING_TRACKERMAIN_H


#include "plane.h"

class TrackerMain {
public:
    void initialize(const string &address, int i);

    std::vector<plane *> planeList() {
        return m_planeList;
    }

    plane *findMainPlane();

private:
    std::vector<plane *> m_planeList;
};


#endif //TRACKING_TRACKERMAIN_H
