cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME oal_manager)
project(${PROJECT_NAME})
include("../../CMakeUtils.cmake")

file(GLOB prj_HEADERS "*.h")
file(GLOB prj_SOURCES "*.cpp")

include_directories(${ENGINE_CODE_PATH)
#include_directories(${CODE_PATH}/3rdparty/fmod/x32/inc")

#link_directories(${CODE_PATH}/3rdparty/fmod/x32/lib)
#link_libraries(fmodex_vc)

add_library(${PROJECT_NAME} SHARED ${prj_SOURCES} ${prj_HEADERS})
add_library(${PROJECT_NAME}_s STATIC ${prj_SOURCES} ${prj_HEADERS})

set_target_properties(${PROJECT_NAME}_s PROPERTIES COMPILE_FLAGS "-DN_STATIC_BUILD")

target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")
set_property(TARGET ${PROJECT_NAME}_s PROPERTY FOLDER "plugins")

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_s 
	RUNTIME DESTINATION ${BIN_PATH}/engine_plugins
	ARCHIVE DESTINATION ${BIN_PATH}/engine_plugins
	LIBRARY DESTINATION ${BIN_PATH}/engine_plugins)