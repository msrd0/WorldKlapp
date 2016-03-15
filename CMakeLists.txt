cmake_minimum_required(VERSION 3.4)
project(WorldKlapp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if (NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
    message("Using default prefix ${CMAKE_INSTALL_PREFIX} (change with -DCMAKE_INSTALL_PREFIX=/prefix)")
endif ()
add_definitions(-DUSE_PREFIX=${CMAKE_INSTALL_PREFIX})
if (NOT USE_HTTP_PREFIX)
    set(USE_HTTP_PREFIX "/")
endif ()
add_definitions(-DUSE_HTTP_PREFIX=${USE_HTTP_PREFIX})

find_package(Qt5Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
add_definitions(${Qt5Core_DEFINITIONS})

find_package(Qt5Network REQUIRED)
include_directories(${Qt5Network_INCLUDE_DIRS})
add_definitions(${Qt5Network_DEFINITIONS})

find_package(Qt5Gui REQUIRED)
include_directories(${Qt5Gui_INCLUDE_DIRS})
add_definitions(${Qt5Gui_DEFINITIONS})

find_package(Qt5Sql REQUIRED)
include_directories(${Qt5Sql_INCLUDE_DIRS})
add_definitions(${Qt5Sql_DEFINITIONS})

add_executable(klapp-competitors
        competitors/main.cpp)
target_link_libraries(klapp-competitors
        Qt5::Core
        Qt5::Network
        Qt5::Gui
        Qt5::Sql)

aux_source_directory(QtWebApp/httpserver QTWEBAPP_HTTPSERVER)
aux_source_directory(QtWebApp/templateengine QTWEBAPP_TEMPLATEENGINE)
include_directories(QtWebApp/httpserver QtWebApp/templateengine)
add_library(QtWebApp SHARED ${QTWEBAPP_HTTPSERVER} ${QTWEBAPP_TEMPLATEENGINE})
target_link_libraries(QtWebApp
        Qt5::Core
        Qt5::Network)

add_executable(klapp-httpd
        httpd/main.cpp httpd/klapprequesthandler.h httpd/klapprequesthandler.cpp)
target_link_libraries(klapp-httpd
        QtWebApp
        Qt5::Sql)

