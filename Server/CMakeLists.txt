cmake_minimum_required(VERSION 3.0.0)
project(PrjServer VERSION 0.1.0)

set(SHARED_DIR "${CMAKE_SOURCE_DIR}/../shared")

set(BOOST_LATEST_DIR            "${SHARED_DIR}/boost_1_65_1")
set(BOOST_LATEST_BIN_DIR        "${BOOST_LATEST_DIR}/stage/lib")
set(BOOST_LATEST_INCLUDE_DIR    "${BOOST_LATEST_DIR}/boost")
set(BOOST_SYSTEM                "${BOOST_LATEST_BIN_DIR}/libboost_system.so")
set(BOOST_FS                    "${BOOST_LATEST_BIN_DIR}/libboost_filesystem.so")
set(BOOST_THREAD                "${BOOST_LATEST_BIN_DIR}/libboost_thread.so")

include_directories(${BOOST_LATEST_INCLUDE_DIR} )

set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu/")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

find_package(Boost 1.65.1 REQUIRED COMPONENTS system filesystem)
include_directories( ${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS})

include(CTest)
enable_testing()

add_executable(PrjServer main.cpp)




target_link_libraries(PrjServer ${Boost_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
