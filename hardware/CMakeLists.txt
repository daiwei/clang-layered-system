###########################################################
# build for `hardware`
###########################################################

set(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/build/hardware/lib)

# include_directories(
#     ${CMAKE_SOURCE_DIR}/hardware/include
# )

link_directories(
    ${CMAKE_SOURCE_DIR}/build/framework/lib
)

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

add_library(hardware SHARED ${SRC_LIST})
target_link_libraries(hardware framework)

add_subdirectory(gps)
