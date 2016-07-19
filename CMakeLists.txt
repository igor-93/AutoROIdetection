cmake_minimum_required(VERSION 3.5)
project(AutoROIdetection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )

include_directories("/home/igorpesic/ClionProjects/AutoROIdetection/src/")

set(SOURCE_FILES
        src/main.cpp
        src/undistort.cpp
        src/generalParams.cpp
        src/cameraParams.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/igorpesic/ClionProjects/AutoROIdetection/Debug/")
add_executable(AutoROIdetection ${SOURCE_FILES})
target_link_libraries( AutoROIdetection ${OpenCV_LIBS} )