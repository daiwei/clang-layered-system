###########################################################
# build for `framework/module`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

add_library(location SHARED ${SRC_LIST})
target_link_libraries(location framework gps)
