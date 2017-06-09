###########################################################
# build for `gateway test`
###########################################################

message(STATUS "###########################################################")
message(STATUS "# gateway test")
message(STATUS "###########################################################")

project(gateway)
#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 2.8)
# set(CMAKE_VERBOSE_MAKEFILE    ON)

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -std=gnu99 -rdynamic -fPIC -g")

include_directories(
    ${CMAKE_SOURCE_DIR}/../hardware/include
    ${CMAKE_SOURCE_DIR}/../framework/include
    ${CMAKE_SOURCE_DIR}/../service/include
    ${CMAKE_SOURCE_DIR}/../vendor/include
)

# set(SRC_LIST)
# aux_source_directory(./ SRC_LIST)
#
# set(SRC_LIST ${SRC_LIST} ${CMAKE_SOURCE_DIR}/../framework/common/list.c)
# set(SRC_LIST test_ringbuf.c ${CMAKE_SOURCE_DIR}/../framework/common/ringbuf.c)
set(SRC_LIST test_ringbuf.c)

link_directories(
    ${CMAKE_SOURCE_DIR}/../vendor/lib-x86
    ${CMAKE_SOURCE_DIR}/../build/framework/lib
)

add_executable(test-runner ${SRC_LIST})
target_link_libraries(test-runner framework criterion)
