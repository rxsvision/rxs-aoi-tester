# ===========================================================================
# rxs-aoi-tester — AOI 测试平台
# Top-level CMake build system
#
# This CMakeLists.txt unifies two subsystems:
#   1. AoiTester/    — CloudCompare-based 3D point cloud application
#   2. rxsFixdev/    — Fixed-device DAL (Data Access Layer) framework
#   3. flowchart2d/  — 2D flowchart engine (QMacVisual integration)
#
# Usage:
#   cmake -B build -S . [options]
#   cmake --build build --config Release
# ===========================================================================
cmake_minimum_required(VERSION 3.16)

project(rxs_aoi_tester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Options — control which subsystems to build
# ---------------------------------------------------------------------------
option(RXS_BUILD_AOI_TESTER  "Build CloudCompare-based AoiTester"     ON)
option(RXS_BUILD_FIXDEV      "Build rxsFixdev DAL framework"           ON)
option(RXS_BUILD_FLOWCHART2D "Build flowchart2d engine"                OFF)

# ---------------------------------------------------------------------------
# MSVC global settings
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/utf-8 /MP /W2 /permissive-)
    add_compile_definitions(
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
endif()

# ---------------------------------------------------------------------------
# Subsystems
# ---------------------------------------------------------------------------
if(RXS_BUILD_AOI_TESTER)
    add_subdirectory(AoiTester)
endif()

if(RXS_BUILD_FIXDEV)
    add_subdirectory(rxsFixdev)
endif()

if(RXS_BUILD_FLOWCHART2D)
    add_subdirectory(flowchart2d)
endif()

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "=== rxs-aoi-tester Configuration ===")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "  AoiTester:        ${RXS_BUILD_AOI_TESTER}")
message(STATUS "  rxsFixdev:        ${RXS_BUILD_FIXDEV}")
message(STATUS "  flowchart2d:      ${RXS_BUILD_FLOWCHART2D}")
message(STATUS "  Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "====================================")
