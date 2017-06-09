###########################################################
# build for `framework`
###########################################################

set(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/build/framework/lib)

# include_directories(
#     ${CMAKE_SOURCE_DIR}/hardware/include
#     ${CMAKE_SOURCE_DIR}/framework/include
#     ${CMAKE_SOURCE_DIR}/vendor/include
# )

link_directories(
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

add_subdirectory(common)
add_subdirectory(module)
