cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME editor_commands)
project(${PROJECT_NAME})

include("../../CMakeUtils.cmake")

file(GLOB prj_HEADERS "*.h")
file(GLOB prj_SOURCES "*.cpp")

include_directories(${ENGINE_CODE_PATH})
include_directories(${CODE_PATH})

add_library(${PROJECT_NAME} SHARED ${prj_SOURCES} ${prj_HEADERS})
add_library(${PROJECT_NAME}_s STATIC ${prj_SOURCES} ${prj_HEADERS})

set_target_properties(${PROJECT_NAME}_s PROPERTIES COMPILE_FLAGS "-DE_STATIC_BUILD")
target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "modules")
set_property(TARGET ${PROJECT_NAME}_s PROPERTY FOLDER "modules")
	
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH}/modules)

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH}/modules)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH}/modules)
endif(MSVC)