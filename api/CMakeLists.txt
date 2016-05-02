find_package(PkgConfig REQUIRED)
pkg_search_module(HTTPD libmicrohttpd REQUIRED)

add_executable(klapp-api main.cpp)
target_link_libraries(klapp-api ${HTTPD_LIBRARIES} Qt5::Core)
