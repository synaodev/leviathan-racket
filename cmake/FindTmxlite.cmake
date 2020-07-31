include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (TMXLITE_INCLUDE_DIR NAMES tmxlite/Config.hpp PATH_SUFFIXES include)
find_library (TMXLITE_LIBRARY NAMES tmxlite PATH_SUFFIXES lib)

find_package_handle_standard_args (
    TMXLITE
    REQUIRED_VARS TMXLITE_LIBRARY TMXLITE_INCLUDE_DIR
)

