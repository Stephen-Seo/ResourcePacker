
find_library(ResourcePacker-s_LIBRARY ResourcePacker-s
    PATHS "@CMAKE_INSTALL_PREFIX@/lib"
    NO_DEFAULT_PATH
)
find_library(ResourcePacker-s_LIBRARIES ResourcePacker-s
    PATHS "@CMAKE_INSTALL_PREFIX@/lib"
    NO_DEFAULT_PATH
)

find_path(ResourcePacker-s_INCLUDE_DIR ResourcePacker.hpp
    PATHS "@CMAKE_INSTALL_PREFIX@/include"
    NO_DEFAULT_PATH
)
find_path(ResourcePacker-s_INCLUDE_DIRS ResourcePacker.hpp
    PATHS "@CMAKE_INSTALL_PREFIX@/include"
    NO_DEFAULT_PATH
)
