###########################################################
# build for `app/demo`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

add_library(demo SHARED ${SRC_LIST})
target_link_libraries(demo framework dl)
