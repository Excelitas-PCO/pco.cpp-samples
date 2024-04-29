cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(samples-pco-cpp
    VERSION 1.0.0
    DESCRIPTION "Simple examples showing the use of pco.cpp"
    LANGUAGES CXX)

set(CMAKE_BUILD_RPATH "\$ORIGIN;\$ORIGIN/lib")
set(CMAKE_INSTALL_RPATH "\$ORIGIN;\$ORIGIN/../lib")

if(UNIX)
    add_compile_definitions(PCO_LINUX)
endif(UNIX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# pco dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/pco)
set(PCO_FOLDER "${CMAKE_SOURCE_DIR}/externals/pco")

add_subdirectory(${CMAKE_SOURCE_DIR}/src/ColorConvertExample)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/FileModeExample)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/MultiCameraExample)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/SimpleExample)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/SimpleExample_CamRam)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/SimpleExample_FIFO)
# Image Viewer requires Qt installation, thus disabled by default
#add_subdirectory(${CMAKE_SOURCE_DIR}/src/ImageViewer)
