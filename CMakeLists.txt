cmake_minimum_required(VERSION 3.5)

project(datetimeedit LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED 
COMPONENTS 
	Widgets
)
LIST(APPEND DATETIMEEDIT_DEMO_LIBRARIES
	Qt${QT_VERSION_MAJOR}::Widgets
)

set(RCC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/datetimeedit.qrc
)

add_subdirectory(src)
add_subdirectory(example)



