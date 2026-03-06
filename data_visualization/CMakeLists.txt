cmake_minimum_required(VERSION 3.16)

add_library(data_visualization STATIC
    DataVisualizer.cpp
    LiveVisualizer.cpp
)

target_include_directories(data_visualization PUBLIC
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../source
)

target_link_libraries(data_visualization PUBLIC
    alife_persistence
)

add_executable(visualizer_tool
    main_visualizer.cpp
)

target_include_directories(visualizer_tool PRIVATE
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../source
)

target_link_libraries(visualizer_tool PRIVATE
    data_visualization
)

add_executable(live_visualizer_tool
    main_live_visualizer.cpp
)

target_include_directories(live_visualizer_tool PRIVATE
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../source
)

target_link_libraries(live_visualizer_tool PRIVATE
    data_visualization
)
