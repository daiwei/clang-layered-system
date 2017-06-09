###########################################################
# build for `framework/module`
###########################################################

set(SRC_LIST)
# aux_source_directory(./ SRC_LIST)

aux_source_directory(${CMAKE_SOURCE_DIR}/vendor/src/zip/ SRC_ZIP)
set(INSTALLER_SRC installer.c ${SRC_ZIP})
add_library(installer SHARED ${INSTALLER_SRC})
target_link_libraries(installer framework m)
