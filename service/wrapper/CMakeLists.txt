###########################################################
# build for `framework/module`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

add_library(lighttpd SHARED ${SRC_LIST})
target_link_libraries(lighttpd m)
