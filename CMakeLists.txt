# For standalone application

cmake_minimum_required(VERSION 3.16)
project(samples2 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt 5 packages
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Multimedia)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(samples2
    main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/waveformwidget.h
    src/waveformwidget.cpp
)

# Link Qt 5 libraries
target_link_libraries(samples2 PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Multimedia
)