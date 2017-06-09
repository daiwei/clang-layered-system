###########################################################
# build for `gateway`
###########################################################

message(STATUS "###########################################################")
message(STATUS "# gateway test")
message(STATUS "###########################################################")

project(test)
cmake_minimum_required(VERSION 3.1)
# set(CMAKE_VERBOSE_MAKEFILE    ON)

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -rdynamic -fPIC -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/../hardware/include
    ${CMAKE_SOURCE_DIR}/../framework/include
    ${CMAKE_SOURCE_DIR}/../service/include
    ${CMAKE_SOURCE_DIR}/../vendor/include
)

add_subdirectory(framework)
