
get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(${SELF_DIR}/ResourcePackerTargets.cmake)

set(ResourcePacker_INCLUDE_DIR ${SELF_DIR}/../../../include)
set(ResourcePacker_INCLUDE_DIRS ${SELF_DIR}/../../../include)
set(ResourcePacker_LIBRARY ${SELF_DIR}/../../../lib/libResourcePacker.so)
set(ResourcePacker_LIBRARIES ${SELF_DIR}/../../../lib/libResourcePacker.so)
