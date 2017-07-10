cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME studio)
project(${PROJECT_NAME})

set(CMAKE_PREFIX_PATH $ENV{QTDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include("../../../CMakeUtils.cmake")

file(GLOB_RECURSE prj_HEADERS "*.h")
file(GLOB_RECURSE prj_SOURCES "*.cpp")
file(GLOB_RECURSE prj_UIS "*.ui")
file(GLOB_RECURSE prj_QRCS "*.qrc")
file(GLOB_RECURSE prj_RCS "*.rc")

add_definitions(-DWIN32)
add_source_group(3rdparty/enet ${CODE_PATH}/3rdparty/enet prj_SOURCES)

find_package(Qt5Core REQUIRED)
find_package(Qt5Help REQUIRED)
find_package(Qt5WebKit REQUIRED)

include_directories(${Qt5Core_INCLUDES})
include_directories(${Qt5Help_INCLUDES})
include_directories(${Qt5WebKit_INCLUDES})

add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Help_DEFINITIONS})
add_definitions(${Qt5WebKit_DEFINITIONS})

include_directories(${PROJECT_BINARY_DIR})
include_directories(./)
include_directories(${CODE_PATH}/3rdparty/enet/include)

link_directories(${BUILD_PATH}/lib/${CMAKE_CFG_INTDIR})

#QT5_ADD_RESOURCES(prj_RC_SRCS ${prj_QRCS})
QT5_WRAP_UI(prj_UI_HDRS ${prj_UIS})
#QT5_WRAP_CPP(prj_MOC_SRCS ${prj_MOCS})

add_executable(${PROJECT_NAME} WIN32 ${prj_HEADERS} ${prj_SOURCES} ${prj_UIS} ${prj_QRCS} ${prj_RCS} ${prj_UI_HDRS})

qt5_use_modules(${PROJECT_NAME} Core Help WebKit)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/studio")
target_link_libraries(${PROJECT_NAME} Qt5::WinMain)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

if(WIN32)
	target_link_libraries(${PROJECT_NAME} shlwapi winmm Ws2_32)
endif(WIN32)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH})
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH})
endif(MSVC)