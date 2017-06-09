###########################################################
# build for `framework/module`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

link_directories(
    ${CMAKE_SOURCE_DIR}/build/hardware/lib
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

# set(SRC_LOCATION location.c)
# add_library(location SHARED ${SRC_LOCATION})
# target_link_libraries(location hardware gps)

set(SRC_POWER power.c)
add_library(power SHARED ${SRC_POWER})
target_link_libraries(power framework)
