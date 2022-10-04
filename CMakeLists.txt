cmake_minimum_required(VERSION 3.6)

project(jiangRpc)

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    message(STATUS "building for: debugging")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -o0 -Wall")
else()
    message(STATUS "building for: release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -o2")
endif()
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)


include_directories(${PROJECT_SOURCE_DIR})


add_subdirectory(jiangRpc/net/http)
add_subdirectory(jiangRpc/net/)
add_subdirectory(jiangRpc/log)
add_subdirectory(examples)


