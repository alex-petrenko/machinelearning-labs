cmake_minimum_required(VERSION 2.8.12.2)

project(machinelearning-lab1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

find_package(OpenCV REQUIRED)
find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(app main.cpp)
target_link_libraries(app ${OpenCV_LIBS})
