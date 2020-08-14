include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_library(PUGIXML_LIBRARIES
    NAMES pugixml
)
find_path(PUGIXML_INCLUDE_DIRS
    NAMES pugixml.hpp
    PATH_SUFFIXES pugixml
)

find_package_handle_standard_args(pugixml DEFAULT_MSG
    PUGIXML_LIBRARIES
    PUGIXML_INCLUDE_DIRS
)
