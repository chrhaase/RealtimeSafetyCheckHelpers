cmake_minimum_required(VERSION 3.24)

project(RealtimeSafetyCheckHelpers
        VERSION 0.1.0
        DESCRIPTION "Helpers for RealtimeSafetyCheck"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE src/ScopedAllocationDetector.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

if (PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(test)
endif ()