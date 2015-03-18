cmake_minimum_required(VERSION 2.8.12.2)

project(machinelearning-labs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

find_package(OpenCV REQUIRED)
find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(lab1_knn lab1/lab1_knn.cpp)
target_link_libraries(lab1_knn ${OpenCV_LIBS})

include_directories(lab2)
add_executable(lab2_titanic lab2/lab2_titanic.cpp lab2/person.cpp)
