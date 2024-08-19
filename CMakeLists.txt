project(spriteprinter)
cmake_minimum_required(VERSION 3.5.0)

include(FindPkgConfig)
find_package(PNG REQUIRED)

if(WITH_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g3 -O0")
endif()

add_executable(
    spriteprinter
        src/spdata.cpp
        src/spriteprinter.cpp
)

target_link_libraries(
    spriteprinter
        png
)
