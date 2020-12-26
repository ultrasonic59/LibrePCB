# CMake version configuration
# (Note: The minimal version 3.5 is shipped with Ubuntu 16.04)
cmake_minimum_required(VERSION 3.5...3.19)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Define project
project(librepcb
    VERSION 0.1.5
    LANGUAGES CXX
)

# Configure module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Global options
option(UNBUNDLE_MUPARSER "Don't use vendored MuParser library." OFF)
option(UNBUNDLE_POLYCLIPPING "Don't use vendored Polyclipping library." OFF)
option(UNBUNDLE_QUAZIP "Don't use vendored QuaZip library." OFF)
option(UNBUNDLE_ALL "Don't use any vendored library." OFF)

# Create a release build by default
include(DefaultBuildType)

# Force static linking for local libs
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Link dynamically against local libraries." FORCE)

# Auto-include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required Qt packages
find_package(Qt5 5.2.0 COMPONENTS Widgets Gui OpenGL Network Sql Svg PrintSupport REQUIRED)

# Find third party libraries
find_package(TypeSafe REQUIRED)
find_package(DelaunayTriangulation REQUIRED)
find_package(Optional REQUIRED)
find_package(QuaZip REQUIRED)
find_package(MuParser REQUIRED)
find_package(Polyclipping REQUIRED)

# Add libs
add_subdirectory(libs/librepcb/common)
add_subdirectory(libs/librepcb/library)
add_subdirectory(libs/librepcb/project)
add_subdirectory(libs/librepcb/workspace)
add_subdirectory(libs/librepcb/projecteditor)
add_subdirectory(libs/librepcb/libraryeditor)
add_subdirectory(libs/librepcb/librarymanager)

# Add apps
add_subdirectory(apps/librepcb)
add_subdirectory(apps/UuidGenerator)
