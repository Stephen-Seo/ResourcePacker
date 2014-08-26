
find_library(ResourcePacker_LIBRARY ResourcePacker
    PATHS @CMAKE_INSTALL_PREFIX@/lib
    NO_DEFAULT_PATH
)
find_library(ResourcePacker_LIBRARIES ResourcePacker
    PATHS @CMAKE_INSTALL_PREFIX@/lib
    NO_DEFAULT_PATH
)

find_path(ResourcePacker_INCLUDE_DIR ResourcePacker.hpp
    PATHS @CMAKE_INSTALL_PREFIX@/include
    NO_DEFAULT_PATH
)
find_path(ResourcePacker_INCLUDE_DIRS ResourcePacker.hpp
    PATHS @CMAKE_INSTALL_PREFIX@/include
    NO_DEFAULT_PATH
)