# simple_simulation/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(SimpleSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Add executable
add_executable(simple_simulation main.cpp)

# Link dependencies
target_link_libraries(simple_simulation PRIVATE
    imgui::imgui
    glfw
    GLEW::GLEW
    OpenGL::GL
)

# Include directories
target_include_directories(simple_simulation PRIVATE
    ${imgui_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)
