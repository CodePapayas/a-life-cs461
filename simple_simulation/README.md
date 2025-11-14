# Simple Organism Simulation

This is a minimal C++ and Dear ImGui application to demonstrate a simple digital organism simulation.

## Author
- **Name:** kai lindskog
- **Date:** November 10, 2025

## How to Build and Run

### Prerequisites
- A C++ compiler (supporting C++17)
- CMake (version 3.10 or higher)
- `vcpkg` for dependency management. Make sure `vcpkg` is installed and integrated.

### Dependencies
The project relies on the following libraries, which should be installed via `vcpkg`:
- `imgui`
- `glfw3`
- `glew`

If you haven't installed them, you can do so with:
```bash
vcpkg install imgui[glfw-binding,opengl3-binding] glfw3 glew
```

### Building the Project
1.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Configure with CMake:**
    You need to point CMake to the `vcpkg` toolchain file. The path to this file might vary depending on your `vcpkg` installation location.

    ```bash
    cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
    ```
    Replace `[path to vcpkg]` with the actual path to your `vcpkg` installation.

3.  **Build the executable:**
    ```bash
    cmake --build .
    ```

### Running the Simulation
After a successful build, you can run the simulation from the `build` directory:

```bash
./simple_simulation
```
