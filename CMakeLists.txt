cmake_minimum_required(VERSION 3.4)
project(WorldKlapp)
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

pkg_search_module(MYSQL REQUIRED mysql++)
include_directories(${MYSQL_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MYSQL_CFLAGS}")

find_package(Boost REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})

add_executable(klapp-competitors 
        competitors/main.cpp)
target_link_libraries(klapp-competitors
        ${MYSQL_LIBRARIES}
        ${BOOST_LIBRARIES})

