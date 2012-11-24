project( spacecraft )
cmake_minimum_required( VERSION 2.6 )

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
set( TEST_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/gtest/main.cpp )

include( filesystem )
include( configure_project )

set( PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
set( SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( BINARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin )

enable_testing()
#create_directory( tests )

add_subdirectory( src Game )

