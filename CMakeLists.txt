project(spriteprinter)
cmake_minimum_required(VERSION 3.5.0)

include(FindPkgConfig)
find_package(PNG REQUIRED)
find_package(
    Qt5
    COMPONENTS
        Widgets
    REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

if(WITH_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
endif()

add_executable(
    spriteprinter
        src/spdata.cpp
        src/spimagearea.cpp
        src/spmainwindow.cpp
        src/spriteprinter.cpp
)

set_target_properties(
    spriteprinter
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
)

target_link_libraries(
    spriteprinter
        png
        Qt5::Widgets
)
