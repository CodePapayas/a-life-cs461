cmake_minimum_required(VERSION 3.16)
project(decision_center_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(decision_center_tests
    tests/test_maths.cpp
    brain.cpp
)

target_include_directories(decision_center_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
)
