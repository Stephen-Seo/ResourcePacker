cmake_minimum_required(VERSION 2.6)
project(ResourcePacker)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(RP_VERSION 1.0.0)

include(GenerateExportHeader)

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

generate_export_header(ResourcePacker)
set_property(TARGET ResourcePacker PROPERTY VERSION ${RP_VERSION})
set_property(TARGET ResourcePacker PROPERTY SOVERSION 1)
set_property(TARGET ResourcePacker PROPERTY INTERFACE_ResourcePacker_MAJOR_VERSION 1)
set_property(TARGET ResourcePacker APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ResourcePacker_MAJOR_VERSION)


add_executable(ResourcePackerExecutable main.cpp)
target_link_libraries(ResourcePackerExecutable ResourcePacker)

install(TARGETS ResourcePacker EXPORT ResourcePackerTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(TARGETS ResourcePackerExecutable DESTINATION bin)

install(
  FILES
    ResourcePacker.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/resourcepacker_export.h"
  DESTINATION
    include
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfigVersion.cmake"
  VERSION ${RP_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ResourcePackerTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerTargets.cmake"
  NAMESPACE RP_CMAKE::
)

configure_file(cmake/ResourcePackerConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfig.cmake"
  COPY_ONLY
)

set(ConfigPackageLocation lib/cmake/ResourcePacker)
install(EXPORT ResourcePackerTargets
  FILE
    ResourcePackerTargets.cmake
  NAMESPACE
    RP_CMAKE::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/ResourcePackerConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
