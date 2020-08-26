include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (FMT_INCLUDE_DIR "fmt/format.h")
find_library (FMT_LIBRARY NAMES "fmt" PATH_SUFFIXES "lib")

find_package_handle_standard_args (
    FMT
    REQUIRED_VARS FMT_LIBRARY FMT_INCLUDE_DIR
)
