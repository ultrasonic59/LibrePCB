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
option(BUILD_TESTS "Build unit tests." ON)
option(UNBUNDLE_FONTOBENE_QT5 "Don't use vendored FontoBeneQt5 library." OFF)
option(UNBUNDLE_GTEST "Don't use vendored GoogleTest library." OFF)
option(UNBUNDLE_HOEDOWN "Don't use vendored Hoedown library." OFF)
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

# Include GNU install dirs variables
include(GNUInstallDirs)

# Find required Qt packages
find_package(Qt5 5.2.0 COMPONENTS Concurrent Gui Network OpenGL PrintSupport Sql Svg Widgets REQUIRED)
if(BUILD_TESTS)
    find_package(Qt5 5.2.0 COMPONENTS Test REQUIRED)
endif()
message(STATUS "Building with Qt${Qt5Core_VERSION}")

# Find third party libraries
find_package(DelaunayTriangulation REQUIRED)
find_package(FontoBeneQt5 REQUIRED)
find_package(MuParser REQUIRED)
find_package(Optional REQUIRED)
find_package(Polyclipping REQUIRED)
find_package(QuaZip REQUIRED)
find_package(TypeSafe REQUIRED)
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
endif()
# Hoedown is only needed on Qt <5.14
if(Qt5Core_VERSION VERSION_LESS 5.14)
    message(STATUS "Qt <5.14 detected, using Hoedown for markdown support")
    find_package(Hoedown REQUIRED)
endif()

# Add libs
add_subdirectory(libs/librepcb/common)
add_subdirectory(libs/librepcb/library)
add_subdirectory(libs/librepcb/project)
add_subdirectory(libs/librepcb/workspace)
add_subdirectory(libs/librepcb/projecteditor)
add_subdirectory(libs/librepcb/libraryeditor)
add_subdirectory(libs/librepcb/librarymanager)
add_subdirectory(libs/librepcb/eagleimport)
add_subdirectory(libs/parseagle)

# Add apps
add_subdirectory(apps/librepcb)
add_subdirectory(apps/librepcb-cli)
add_subdirectory(apps/UuidGenerator)
add_subdirectory(apps/EagleImport)

# Add unittests
if(BUILD_TESTS)
    add_subdirectory(tests/unittests)
endif()

# Install target for shared files
file(GLOB SHARE_DIRS
    LIST_DIRECTORIES true
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    share/*
)
foreach(d ${SHARE_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${d}")
        install(DIRECTORY ${d} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})
    endif()
endforeach()
