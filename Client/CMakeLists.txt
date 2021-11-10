cmake_minimum_required(VERSION 3.10.2)
project(client VERSION 0.1.0)


find_package(Boost 1.65.1 REQUIRED COMPONENTS thread system filesystem  )
include_directories( ${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS})


add_executable(client main.cpp)

target_link_libraries(client ${Boost_LIBRARIES})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
