include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (STB_INCLUDE_DIR stb.h)

find_package_handle_standard_args (
    STB 
    REQUIRED_VARS STB_INCLUDE_DIR
)