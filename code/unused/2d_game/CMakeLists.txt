cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME 2d_game)
project(${PROJECT_NAME})

include("../../../CMakeUtils.cmake")

file(GLOB_RECURSE prj_HEADERS "*.h")
file(GLOB_RECURSE prj_SOURCES "*.cpp")

include_directories(${ENGINE_CODE_PATH})

add_library(${PROJECT_NAME} SHARED ${prj_SOURCES} ${prj_HEADERS})
add_library(${PROJECT_NAME}_s STATIC ${prj_SOURCES} ${prj_HEADERS})

target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})

set_target_properties(${PROJECT_NAME}_s PROPERTIES COMPILE_FLAGS "-DN_STATIC_BUILD")
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "samples")
set_property(TARGET ${PROJECT_NAME}_s PROPERTY FOLDER "samples")

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_s 
	RUNTIME DESTINATION ${BIN_PATH}/engine_plugins
	ARCHIVE DESTINATION ${BIN_PATH}/engine_plugins
	LIBRARY DESTINATION ${BIN_PATH}/engine_plugins)