//
// Created by kardasland on 3/24/24.
//

#ifndef TRACKING_TRACKERMAIN_H
#define TRACKING_TRACKERMAIN_H


#include "plane.h"

class TrackerMain {
public:
    void initialize();
    std::vector<plane*> planeList() {
        return m_planeList;
    }
    plane* findMainPlane();
private:
    std::vector<plane*> m_planeList;
};


#endif //TRACKING_TRACKERMAIN_H
