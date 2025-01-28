cmake_minimum_required(VERSION 3.16)

project(SVG-Dereferencer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(SVG-Dereferencer
  main.cpp
  frameobject.h frameobject.cpp
)
target_link_libraries(SVG-Dereferencer Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(SVG-Dereferencer Qt${QT_VERSION_MAJOR}::Xml)

include(GNUInstallDirs)
install(TARGETS SVG-Dereferencer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
