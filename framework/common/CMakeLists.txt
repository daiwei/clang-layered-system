###########################################################
# build for `framework/common`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

link_directories(
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

# add_library(common STATIC ${SRC_LIST})
add_library(framework SHARED ${SRC_LIST})
# target_link_libraries(framework dbus)
target_link_libraries(framework dbus-1 dl)
