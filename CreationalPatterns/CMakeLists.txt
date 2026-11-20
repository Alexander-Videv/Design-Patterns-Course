cmake_minimum_required(VERSION 3.14)
project(MyFigureProject VERSION 1.0 DESCRIPTION "C++ Figure Project with GTest")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the implementation directory
add_subdirectory(src)

# Enable CTest and add the tests directory
enable_testing()
add_subdirectory(tests)