###########################################################
# build for `app`
###########################################################

set(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/build/app)

# include_directories(
#     ${CMAKE_SOURCE_DIR}/framework/include
#     ${CMAKE_SOURCE_DIR}/vendor/include
# )

link_directories(
    ${CMAKE_SOURCE_DIR}/build/framework/lib
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

add_subdirectory(demo)
