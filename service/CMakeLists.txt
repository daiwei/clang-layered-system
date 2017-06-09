###########################################################
# build for `service`
###########################################################

set(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/build/service/lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/service/include
    ${CMAKE_SOURCE_DIR}/vendor/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

add_subdirectory(location)
add_subdirectory(installer)
add_subdirectory(wrapper)
