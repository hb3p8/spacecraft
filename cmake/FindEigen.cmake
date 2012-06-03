include(FindPackageHandleStandardArgs)

find_path( EIGEN_INCLUDE_DIR  "Eigen/Core" PATH_SUFFIXES /eigen2 )  

unset( EIGEN_FOUND )
if( ${EIGEN_INCLUDE_DIR} STREQUAL EIGEN_INCLUDE_DIR-NOTFOUND )
  set( EIGEN_FOUND FALSE )
  if( NOT EIGEN_FOUND AND NOT EIGEN_FIND_QUIETLY AND Eigen_FIND_REQUIRED )
    message( FATAL_ERROR "Cannot find EIGEN." )
  endif()
else()
  set( EIGEN_FOUND TRUE )
endif()