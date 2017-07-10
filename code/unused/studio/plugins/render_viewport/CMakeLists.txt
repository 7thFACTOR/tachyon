cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME render_viewport)
project(${PROJECT_NAME})
include("../../../../CMakeUtils.cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE prj_HEADERS "*.h")
file(GLOB_RECURSE prj_SOURCES "*.cpp")
file(GLOB_RECURSE prj_UIS "*.ui")
file(GLOB_RECURSE prj_QRCS "*.qrc")
file(GLOB_RECURSE prj_RCS "*.rc")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Help REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebKit REQUIRED)

include_directories(${PROJECT_BINARY_DIR})

link_directories(${BUILD_PATH}/lib/${CMAKE_CFG_INTDIR})

QT5_WRAP_UI(prj_UI_HDRS ${prj_UIS})

add_library(${PROJECT_NAME} SHARED ${prj_SOURCES} ${prj_HEADERS} ${prj_UIS} ${prj_QRCS} ${prj_RCS} ${prj_UI_HDRS})

qt5_use_modules(${PROJECT_NAME} Core Help Network WebKit)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/studio/plugins")

if(WIN32)
	target_link_libraries(${PROJECT_NAME} Qt5::WinMain)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH}/studio_plugins)

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH}/studio_plugins)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH}/studio_plugins)
endif(MSVC)

add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CODE_PATH}/tools/studio/plugins/${PROJECT_NAME}/${PROJECT_NAME}.plugin ${BIN_PATH}/studio_plugins/${PROJECT_NAME}.plugin)