cmake_minimum_required(VERSION 3.16)
project(a_life_cs461 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DECISION_CENTER_SOURCES
    source/entity/decision_center/brain.cpp
    source/entity/decision_center/entity.cpp
    source/entity/decision_center/biology.cpp
)

set(PERCEPTION_MOVEMENT_SOURCES
    source/entity/perception_movement/perception.cpp
    source/entity/perception_movement/movement.cpp
)

set(MAIN_SOURCES
    main.cpp
    source/simulation/Simulation.cpp
    source/environment/Environment.cpp
    source/environment/resource_node.cpp
)

add_executable(main
    ${MAIN_SOURCES}
    ${PERCEPTION_MOVEMENT_SOURCES}
    ${DECISION_CENTER_SOURCES}
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/source
    ${PROJECT_SOURCE_DIR}/source/entity/decision_center
    ${PROJECT_SOURCE_DIR}/source/entity/perception_movement
)
