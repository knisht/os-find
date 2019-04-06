cmake_minimum_required(VERSION 2.8)

project (findy)

include_directories(${PROJECT_SOURCE_DIR})

SET(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fsanitize=address,undefined)

add_executable(findy
    ${SOURCE_DIR}/driver.cpp
    ${SOURCE_DIR}/argument_config.h
    ${SOURCE_DIR}/entry_manager.h
    ${SOURCE_DIR}/entry_manager.cpp)

