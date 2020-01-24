cmake_minimum_required(VERSION 3.11)
project(mapmaker)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(mapmaker src/main.cpp
  src/MainWindow.cpp src/MainWindow.h
  src/MapWidget.cpp src/MapWidget.h
  src/floatfield.cpp src/floatfield.h)


target_link_libraries(mapmaker Qt5::Widgets profiler)
