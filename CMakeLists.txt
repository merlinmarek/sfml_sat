cmake_minimum_required(VERSION 2.8.12)
project(sfml_sat)

enable_testing()

# this can be replaced with target_compile_features if cmake version >= 3.1 is available
add_compile_options(-std=c++11) 

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIR})

add_subdirectory(sfml_sat)
