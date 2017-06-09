###########################################################
# build for `hardware/gps`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

add_library(hw.gps SHARED ${SRC_LIST})
target_link_libraries(hw.gps framework m)
