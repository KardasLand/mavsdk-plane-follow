cmake_minimum_required(VERSION 3.7)
project(tracking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MAVSDK
add_executable(tracking main.cpp
        referenceFiles/a.cpp
        plane.cpp
        plane.h
        TrackerMain.cpp
        TrackerMain.h
        referenceFiles/b.cpp)

find_package(MAVSDK REQUIRED)

target_link_libraries(tracking
    MAVSDK::mavsdk
)