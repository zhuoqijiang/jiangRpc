cmake_minimum_required(VERSION 2.6)

project(dbserver)

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
#find_path(TCMALLOC_INCLUDE_DIR ../gperftools/heap-profiler.h)
#find_library(TCMALLOC_LIBRARY NAMES tcmalloc HINTS ../gperftools/)


#if(BOOST_CONTEXT) 
#	message(STATUS "boost no found")
#ENDIF()
#if(TCMALLOC_LIBRARY)
#	message(STATUS "found tcmalloc")
#	target_link_libraries(${PROJECT_NAME} -ltcmalloc)
#else()
#	message(STATUS "no found tcmalloc")
#endif()

add_executable(${PROJECT_NAME} test.cpp)

add_subdirectory(net/http)
add_subdirectory(net/)

target_link_libraries(${PROJECT_NAME} dbserver_http)
