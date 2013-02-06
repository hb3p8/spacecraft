include(FindPackageHandleStandardArgs)

find_path( MYGUI_INCLUDE_DIR "MyGUI.h" )
find_library( MYGUI_LIBRARIES MyGUIEngine )

find_package_handle_standard_args(MYGUI DEFAULT_MSG MYGUI_LIBRARIES MYGUI_INCLUDE_DIR )
