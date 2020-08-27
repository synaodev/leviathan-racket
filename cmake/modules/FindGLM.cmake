include (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

find_path (GLM_INCLUDE_DIR "glm/glm.hpp")

find_package_handle_standard_args (
    GLM
    REQUIRED_VARS GLM_INCLUDE_DIR
)
