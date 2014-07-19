
get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(${SELF_DIR}/ResourcePackerTargets.cmake)

set(ResourcePacker_INCLUDE_DIR ${SELF_DIR}/../../../include)
set(ResourcePacker_INCLUDE_DIRS ${SELF_DIR}/../../../include)
set(ResourcePacker_LIBRARY ${SELF_DIR}/../../../lib/libResourcePacker.so)
set(ResourcePacker_LIBRARIES ${SELF_DIR}/../../../lib/libResourcePacker.so)

get_filename_component(ResourcePacker_INCLUDE_DIR ${ResourcePacker_INCLUDE_DIR} ABSOLUTE)
get_filename_component(ResourcePacker_INCLUDE_DIRS ${ResourcePacker_INCLUDE_DIRS} ABSOLUTE)
get_filename_component(ResourcePacker_LIBRARY ${ResourcePacker_LIBRARY} ABSOLUTE)
get_filename_component(ResourcePacker_LIBRARIES ${ResourcePacker_LIBRARIES} ABSOLUTE)

message(STATUS "Found ResourcePacker at ${ResourcePacker_INCLUDE_DIR}")
