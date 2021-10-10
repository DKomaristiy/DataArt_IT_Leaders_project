cmake_minimum_required(VERSION 3.0.0)
project(client VERSION 0.1.0)


set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu/")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
#set(Boost_NO_BOOST_CMAKE OFF)
#set(Boost_USE_DEBUG_LIBS  ON) 


add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

find_package(Boost 1.65.1 REQUIRED COMPONENTS date_time system filesystem)
include_directories( ${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS})

include(CTest)
enable_testing()

add_executable(client main.cpp)

target_link_libraries(client ${Boost_LIBRARIES})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
