###########################################################
# build for `gateway`
###########################################################

message(STATUS "###########################################################")
message(STATUS "# gateway")
message(STATUS "###########################################################")

project(gateway)
#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 2.8)
# set(CMAKE_VERBOSE_MAKEFILE    ON)

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -std=gnu99 -rdynamic -fPIC -g")
# set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -std=gnu99 -rdynamic -fPIC -g -O0")
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/build/init)

add_custom_target(move-vendor ALL
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/build/vendor
    COMMAND cp -rf ${CMAKE_SOURCE_DIR}/vendor/lib ${CMAKE_SOURCE_DIR}/build/vendor
    COMMENT "Move `vendor` to `build`..."
)

include_directories(
    ${CMAKE_SOURCE_DIR}/hardware/include
    ${CMAKE_SOURCE_DIR}/framework/include
    ${CMAKE_SOURCE_DIR}/service/include
    ${CMAKE_SOURCE_DIR}/vendor/include
)

link_directories(
    /usr/lib/gateway/service/lib
    /usr/lib/gateway/framework/lib
    /usr/lib/gateway/hardware/lib
    /usr/lib/gateway/vendor/lib
)

add_subdirectory(hardware)
add_subdirectory(framework)
add_subdirectory(service)
add_subdirectory(loader)
# add_subdirectory(app)
