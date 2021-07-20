# MINIMUM REQUIRED
cmake_minimum_required(VERSION 3.19)

# PROJECT
project(SilenceClient 
        VERSION 1.0.0 
        LANGUAGES CXX)

# WINDOWS
if (WIN32)
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
endif()

# CXX STANDARD
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# conan
set(CONAN_DISABLE_CHECK_COMPILER ON)
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# PATH VARS
set(APP_DIR ${CMAKE_SOURCE_DIR}/app)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

# ADD SUBDIRECTORIES
add_subdirectory(${VENDOR_DIR})
add_subdirectory(${SRC_DIR})
add_subdirectory(${APP_DIR})