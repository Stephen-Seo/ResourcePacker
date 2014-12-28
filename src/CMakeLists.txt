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


add_library(ResourcePacker SHARED ResourcePacker.cpp)
add_library(ResourcePacker-s STATIC ResourcePacker.cpp)

generate_export_header(ResourcePacker)
set_property(TARGET ResourcePacker PROPERTY VERSION ${RP_VERSION})
set_property(TARGET ResourcePacker PROPERTY SOVERSION 1)
set_property(TARGET ResourcePacker PROPERTY INTERFACE_ResourcePacker_MAJOR_VERSION 1)
set_property(TARGET ResourcePacker APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ResourcePacker_MAJOR_VERSION)

generate_export_header(ResourcePacker-s)
set_property(TARGET ResourcePacker-s PROPERTY VERSION ${RP_VERSION})
#set_property(TARGET ResourcePacker-s PROPERTY SOVERSION 1)
set_property(TARGET ResourcePacker-s PROPERTY INTERFACE_ResourcePacker-s_MAJOR_VERSION 1)
set_property(TARGET ResourcePacker-s APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ResourcePacker-s_MAJOR_VERSION)


add_executable(ResourcePackerExecutable main.cpp)
target_link_libraries(ResourcePackerExecutable ResourcePacker)

install(TARGETS ResourcePacker EXPORT ResourcePackerTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(TARGETS ResourcePacker-s EXPORT ResourcePacker-sTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(TARGETS ResourcePackerExecutable DESTINATION bin)

install(
  FILES
    ResourcePacker.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/resourcepacker_export.h"
    "${CMAKE_CURRENT_BINARY_DIR}/resourcepacker-s_export.h"
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

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker-s/ResourcePacker-sConfigVersion.cmake"
  VERSION ${RP_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ResourcePackerTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerTargets.cmake"
)

export(EXPORT ResourcePacker-sTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker-s/ResourcePacker-sTargets.cmake"
)

configure_file(cmake/ResourcePackerConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfig.cmake"
  @ONLY
)

configure_file(cmake/ResourcePacker-sConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker-s/ResourcePacker-sConfig.cmake"
  @ONLY
)

export(PACKAGE ResourcePacker)
export(PACKAGE ResourcePacker-s)

set(ConfigPackageLocation lib/cmake/ResourcePacker)
set(ConfigPackageLocation-s lib/cmake/ResourcePacker-s)

install(EXPORT ResourcePackerTargets
  FILE
    ResourcePackerTargets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(EXPORT ResourcePacker-sTargets
  FILE
    ResourcePacker-sTargets.cmake
  DESTINATION
    ${ConfigPackageLocation-s}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker/ResourcePackerConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker-s/ResourcePacker-sConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ResourcePacker-s/ResourcePacker-sConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation-s}
  COMPONENT
    Devel
)
