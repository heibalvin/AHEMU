cmake_minimum_required(VERSION 3.20)
project(GBEMU CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Include)

# 1. Compile Emulator Module Logic
add_library(GBEMUCORE STATIC
    Source/GBAPP.cpp    
    Source/GBEMU.cpp
    Source/GBBUS.cpp
    Source/GBDSK.cpp
    Source/GBCPU.cpp
    Source/GBVDP.cpp
    Source/GBJOY.cpp
    Source/GBAPU.cpp
)

# 2. Compile Native Window App Driver Target
add_executable(GBEMU 
    Source/main.cpp
)

find_package(SDL3 REQUIRED)
target_link_libraries(GBEMUCORE PRIVATE SDL3::SDL3)
target_link_libraries(GBEMU PRIVATE GBEMUCORE SDL3::SDL3)