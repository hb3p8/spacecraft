include(FindPackageHandleStandardArgs)

find_path( MYGUIOPENGL_INCLUDE_DIR "MyGUI_OpenGLPlatform.h" )
find_library( MYGUIOPENGL_LIBRARIES MyGUI.OpenGLPlatform )

find_package_handle_standard_args(MYGUIOPENGL DEFAULT_MSG MYGUIOPENGL_LIBRARIES MYGUIOPENGL_INCLUDE_DIR )
