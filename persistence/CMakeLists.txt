cmake_minimum_required(VERSION 3.16)
project(alife_persistence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------
# Find PostgreSQL (libpq)
#   On macOS with Homebrew:  brew install postgresql
#   On Linux:                apt install libpq-dev  /  dnf install libpq-devel
# ---------------------------------------------------------------
find_package(PostgreSQL REQUIRED)

# ---------------------------------------------------------------
# Optional: zlib for genome compression
#   Enabled by default if found; disable with -DALIFE_USE_ZLIB=OFF
# ---------------------------------------------------------------
option(ALIFE_USE_ZLIB "Compress genome data with zlib" ON)
if(ALIFE_USE_ZLIB)
    find_package(ZLIB)
    if(ZLIB_FOUND)
        message(STATUS "zlib found — genome compression enabled")
        add_compile_definitions(ALIFE_USE_ZLIB)
    else()
        message(WARNING "zlib not found — genome data stored uncompressed")
        set(ALIFE_USE_ZLIB OFF)
    endif()
endif()

# ---------------------------------------------------------------
# Persistence library  (db_connector + save_manager + auto_save)
# ---------------------------------------------------------------
add_library(alife_persistence STATIC
    ../src/db_connector.cpp
    ../src/resource_node.cpp
    ../src/save_manager.cpp
    ../src/auto_save.cpp
)

target_include_directories(alife_persistence
    PUBLIC
        ${PROJECT_SOURCE_DIR}/../include
        ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(alife_persistence
    PUBLIC
        ${PostgreSQL_LIBRARIES}
        $<$<BOOL:${ALIFE_USE_ZLIB}>:ZLIB::ZLIB>
)

# ---------------------------------------------------------------
# Integration test for the persistence layer
# ---------------------------------------------------------------
add_executable(test_persistence
    ../test/test_persistence.cpp
)

target_include_directories(test_persistence
    PRIVATE
        ${PROJECT_SOURCE_DIR}/../include
        ${PostgreSQL_INCLUDE_DIRS}
)

target_compile_definitions(test_persistence PRIVATE
    ALIFE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}/.."
)

target_link_libraries(test_persistence
    PRIVATE
        alife_persistence
)

# ---------------------------------------------------------------
# Auto-save smoke test
# ---------------------------------------------------------------
add_executable(test_auto_save
    ../test/test_auto_save.cpp
)

target_include_directories(test_auto_save
    PRIVATE
        ${PROJECT_SOURCE_DIR}/../include
        ${PostgreSQL_INCLUDE_DIRS}
)

target_compile_definitions(test_auto_save PRIVATE
    ALIFE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}/.."
)

target_link_libraries(test_auto_save
    PRIVATE
        alife_persistence
)

# ---------------------------------------------------------------
# Example: how to add the decision_center tests alongside
# (mirrors the existing decision_center/CMakeLists.txt)
# ---------------------------------------------------------------
# add_subdirectory(../decision_center ${CMAKE_BINARY_DIR}/decision_center)
