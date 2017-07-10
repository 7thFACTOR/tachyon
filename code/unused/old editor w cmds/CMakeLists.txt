cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME editor)
project(${PROJECT_NAME})

include("../CMakeUtils.cmake")

file(GLOB prj_HEADERS "*.h")
file(GLOB prj_SOURCES "*.cpp")

if(WIN32)
	enable_language(RC)
	file(GLOB prj_RES "*.rc")
	source_group("res" FILES "*.rc")
endif(WIN32)

include_directories(${CODE_PATH})
include_directories(${ENGINE_CODE_PATH})

if(WIN32)
	link_libraries(opengl32)

if(USE_FMOD)
	if(CMAKE_CL_64)
		link_libraries(fmodex64_vc)
	else(CMAKE_CL_64)
		link_libraries(fmodex_vc)
	endif(CMAKE_CL_64)
endif(USE_FMOD)

if(USE_PHYSX)
	if(CMAKE_CL_64)
		link_libraries(NxCharacter PhysXCooking64 PhysXCore64 PhysXLoader64)
	else(CMAKE_CL_64)
		link_libraries(NxCharacter PhysXCooking PhysXCore PhysXLoader)
	endif(CMAKE_CL_64)
endif(USE_PHYSX)

endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 ${prj_SOURCES} ${prj_HEADERS} ${prj_RES})
add_executable(${PROJECT_NAME}_s WIN32 ${prj_SOURCES} ${prj_HEADERS} ${prj_RES})

target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})

if(WIN32)
	target_link_libraries(${PROJECT_NAME}_s debug zlibstaticd optimized zlibstatic)
	target_link_libraries(${PROJECT_NAME}_s debug ${BASE_NAME}_s_d optimized ${BASE_NAME}_s)
	target_link_libraries(${PROJECT_NAME}_s debug ${ENGINE_NAME}_s_d optimized ${ENGINE_NAME}_s)
	target_link_libraries(${PROJECT_NAME}_s shlwapi winmm Ws2_32)
endif(WIN32)

set_target_properties(${PROJECT_NAME}_s PROPERTIES COMPILE_FLAGS "-DB_STATIC_BUILD -DE_STATIC_BUILD")
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(${PROJECT_NAME}_s PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "editor")
set_property(TARGET ${PROJECT_NAME}_s PROPERTY FOLDER "editor")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})
set_target_properties(${PROJECT_NAME}_s PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH})
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH})
	set_target_properties(${PROJECT_NAME}_s PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH})
	set_target_properties(${PROJECT_NAME}_s PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH})
endif(MSVC)

set_msvc_working_dir(${PROJECT_NAME})