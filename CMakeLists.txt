cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

project(fmseq)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCES
	src/audio.cpp
	src/main.cpp
	src/gui.cpp
)

add_executable(fmseq ${SOURCES})
target_link_libraries(fmseq ${SDL2_LIBRARIES})