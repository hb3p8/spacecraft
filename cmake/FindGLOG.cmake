include(FindPackageHandleStandardArgs)

find_path( GLOG_INCLUDE_DIR "glog/logging.h" )
find_library( GLOG_LIBRARIES glog ) 

find_package_handle_standard_args(GLOG DEFAULT_MSG GLOG_LIBRARIES GLOG_INCLUDE_DIR )