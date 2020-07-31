include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (NLOHMANN_JSON_INCLUDE_DIR NAMES nlohmann/json.hpp PATH_SUFFIXES include)

find_package_handle_standard_args (
    NLOHMANN_JSON
    REQUIRED_VARS NLOHMANN_JSON_INCLUDE_DIR
)

