cmake_minimum_required(VERSION 3.24)
project(WouoUI)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


file(GLOB SRCS_EMULATOR
        u8g2/csrc/*.c u8g2/sys/sdl/common/*.c u8g2/cppsrc/*.cpp
        Arduino/*.c Arduino/*.cpp
        *.c *.cpp)
include_directories(u8g2/csrc/)
include_directories(u8g2/cppsrc/)
include_directories(Arduino/)
include_directories(.)


include_directories(.)
add_executable(WouoUI ${SRCS_EMULATOR})


target_link_libraries(WouoUI ${SDL2_LIBRARIES})

