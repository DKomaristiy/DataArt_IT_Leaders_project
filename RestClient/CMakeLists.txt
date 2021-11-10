cmake_minimum_required(VERSION 3.10)
project(RestClient)


set(CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/)
find_library(cpprestsdk-lib cpprest)
message(STATUS "cpprestsdk-lib = ${cpprestsdk-lib}")

find_package(cpprestsdk REQUIRED)

add_executable(RestClient main.cpp)
target_link_libraries(RestClient PRIVATE cpprestsdk::cpprest)



