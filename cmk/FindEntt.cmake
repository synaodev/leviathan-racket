include(FindPackageHandleStandardArgs)

# Search for the header file
find_path(ENTT_INCLUDE_DIR NAMES entt/entt.hpp PATH_SUFFIXES include)

# Did we find everything we need?
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DEFAULT_MSG ENTT_INCLUDE_DIR)