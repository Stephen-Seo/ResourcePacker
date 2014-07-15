cmake_minimum_required(VERSION 2.6)
project(ResourcePacker)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D NDEBUG")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif(MSVC)
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug', none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

if(NOT DEFINED BUILD_SHARED_LIBS)
    message(STATUS "Setting BUILD_SHARED_LIBS to 'True', none was specified.")
    set(BUILD_SHARED_LIBS True CACHE STRING "Choose whether or not to build lib as shared." FORCE)
    set_property(CACHE BUILD_SHARED_LIBS PROPERTY STRINGS "True" "False")
endif()

add_library(ResourcePacker ResourcePacker.cpp)

add_executable(ResourcePackerExecutable main.cpp)

target_link_libraries(ResourcePackerExecutable ResourcePacker)


install(TARGETS ResourcePacker DESTINATION lib)
install(TARGETS ResourcePackerExecutable DESTINATION bin)
install(FILES ResourcePacker.hpp DESTINATION usr/include)

