cmake_minimum_required(VERSION 3.5)
project(AutoROIdetection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )

set(SOURCE_FILES main.cpp)
add_executable(AutoROIdetection ${SOURCE_FILES})
target_link_libraries( AutoROIdetection ${OpenCV_LIBS} )