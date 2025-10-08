cmake_minimum_required(VERSION 3.22.1)

project("edgedetection")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv/sdk/native/jni)
find_package(OpenCV REQUIRED)

# Add source files
add_library(
    edgedetection
    SHARED
    native-lib.cpp
    edge_processor.cpp
)

# Include directories
target_include_directories(
    edgedetection
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(
    edgedetection
    ${OpenCV_LIBS}
    android
    log
    jnigraphics
)