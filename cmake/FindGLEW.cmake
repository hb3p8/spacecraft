include(FindPackageHandleStandardArgs)

find_path( GLEW_INCLUDE_DIR "GL/glew.h" )
find_library( GLEW_LIBRARIES GLEW )

find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_LIBRARIES GLEW_INCLUDE_DIR )