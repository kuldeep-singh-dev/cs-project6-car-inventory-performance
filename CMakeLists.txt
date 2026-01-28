cmake_minimum_required(VERSION 3.10)
project(DealerDrive)
set(CMAKE_CXX_STANDARD 20)
add_library(MainLibrary backend/src/TestDrive/TestDrive.cpp backend/src/Library/crow_all.h)
add_executable(Main backend/src/Main/Main.cpp)
target_link_libraries(Main MainLibrary)

