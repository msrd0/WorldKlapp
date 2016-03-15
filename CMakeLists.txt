cmake_minimum_required(VERSION 3.4)
project(WorldKlapp)
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if (NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
    message("Using default prefix ${CMAKE_INSTALL_PREFIX} (change with -DCMAKE_INSTALL_PREFIX=/prefix)")
endif()
add_definitions(-DUSE_PREFIX=${CMAKE_INSTALL_PREFIX})
if (NOT USE_HTTP_PREFIX)
    set(USE_HTTP_PREFIX "/")
endif()
add_definitions(-DUSE_HTTP_PREFIX=${USE_HTTP_PREFIX})

pkg_search_module(MYSQL REQUIRED mysql++)
include_directories(${MYSQL_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYSQL_CFLAGS}")

find_package(Boost REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})

find_package(Qt5Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
add_definitions(${Qt5Core_DEFINITIONS})
find_package(Qt5Network REQUIRED)
include_directories(${Qt5Network_INCLUDE_DIRS})
add_definitions(${Qt5Network_DEFINITIONS})
find_package(Qt5Gui REQUIRED)
include_directories(${Qt5Gui_INCLUDE_DIRS})
add_definitions(${Qt5Gui_DEFINITIONS})

add_executable(klapp-competitors 
        competitors/main.cpp)
target_link_libraries(klapp-competitors
        ${MYSQL_LIBRARIES}
        ${BOOST_LIBRARIES}
	Qt5::Core
	Qt5::Network
	Qt5::Gui)

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
	${MYSQL_LIBRARIES}
        ${BOOST_LIBRARIES})

