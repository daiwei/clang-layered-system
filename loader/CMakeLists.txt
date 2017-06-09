###########################################################
# build for `initd` & `loader`
###########################################################

set(SRC_LIST)
aux_source_directory(./ SRC_LIST)

include_directories(
    ${CMAKE_SOURCE_DIR}/framework/include
    ${CMAKE_SOURCE_DIR}/vendor/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/build/framework/lib
    ${CMAKE_SOURCE_DIR}/vendor/lib
)

aux_source_directory(${CMAKE_SOURCE_DIR}/vendor/src/iniparser/ SRC_INIPARSER)
set(INITD_SRC initd.c ${SRC_INIPARSER})
add_executable(initd ${INITD_SRC})
target_link_libraries(initd framework dl)

set(FRW_LOADER_SRC framework_loader.c ${SRC_INIPARSER})
add_executable(frw-loader ${FRW_LOADER_SRC})
# target_link_libraries(frw-loader framework dbus pthread dl)
target_link_libraries(frw-loader framework dbus-1 pthread dl)

set(SRV_LOADER_SRC service_loader.c ${SRC_INIPARSER})
add_executable(srv-loader ${SRV_LOADER_SRC})
# target_link_libraries(srv-loader framework dbus pthread dl)
target_link_libraries(srv-loader framework dbus-1 pthread dl)

set(APP_LOADER_SRC app_loader.c ${SRC_INIPARSER})
add_executable(app-loader ${APP_LOADER_SRC})
# target_link_libraries(app-loader framework dbus pthread dl)
target_link_libraries(app-loader framework dbus-1 pthread dl)
