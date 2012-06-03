#
# USE_RELEASE Builds program in release mode
# USE_DEBUG Builds porgram with debug information
#
macro( configure_compiler ${USE_RELEASE} ${USE_DEBUG} )

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions( -march=native -Wall -Wwrite-strings -Wno-long-long -Werror )

endif()
if( ${USE_RELEASE} )
  if( ${USE_DEBUG} )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" )
  else()
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}" )
  endif()
else()
 set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" )
 set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" )
endif()

endmacro()

macro( configure_project )
set( USE_DEBUG ${LIBRARY_NAME}_USE_DEBUG )
set( USE_RELEASE ${LIBRARY_NAME}_USE_RELEASE )
configure_compiler( USE_RELEASE USE_DEBUG )
endmacro()

#  INPUT VARIABLES:
#
#  NAME - name of current project
#  OPTIONS - FIND OPTIONS( REQUIRED AND QUIET - see CMake find_package documentation.
#  ${NAME}_BOOST_COMPONENTS - used additional boost components
#  ${NAME}_QT_LIBRARIES - if you use additional qt libraries - write them here.
#
#  OUTPUT_VARIABLES
#
#  ${NAME}_PREREQUESTS_INCLUDE_DIR - include directories for prerequests of current project
#  ${NAME}_PREREQUESTS_LIBRARIES - libraries for prerequests of current project.
#

macro( find_prerequests NAME OPTIONS ${ARGC} )
  
  set( ${NAME}_PREREQUESTS_FOUND TRUE )

  foreach( PACKAGE ${ARGN} )


    if( ${PACKAGE} STREQUAL Boost )
       find_package( Boost ${OPTIONS} COMPONENTS ${${NAME}_BOOST_COMPONENTS} )
    else()
      find_package( ${PACKAGE} ${OPTIONS} )
    endif()

    if( ${PACKAGE} STREQUAL Qt4 ) 

      foreach( LIBRARY ${${NAME}_QT_LIBRARIES} )
        if( ${LIBRARY} STREQUAL "QtOpenGL" )
          set( QT_USE_QTOPENGL TRUE )
        endif()
      endforeach( LIBRARY )

      include( ${QT_USE_FILE} )
      set( PACKAGE Qt )
    endif()

    string( TOUPPER ${PACKAGE} UPPER_PACKAGE )

    if( NOT ${PACKAGE}_FOUND AND NOT ${UPPER_PACKAGE}_FOUND )
      set( ${NAME}_PREREQUESTS_FOUND FALSE )
    endif()
 
    list( APPEND ${PACKAGE}_INCLUDE_DIR ${${UPPER_PACKAGE}_INCLUDE_DIR} )
    list( APPEND ${PACKAGE}_LIBRARIES ${${UPPER_PACKAGE}_LIBRARIES} )

    list( APPEND ${NAME}_PREREQUESTS_INCLUDE_DIR ${${PACKAGE}_INCLUDE_DIR} )
    list( APPEND ${NAME}_PREREQUESTS_LIBRARIES ${${PACKAGE}_LIBRARIES} )
 
  endforeach( PACKAGE )

  if( DEFINED ${NAME}_PREREQUESTS_INCLUDE_DIR )
    list( REMOVE_DUPLICATES ${NAME}_PREREQUESTS_INCLUDE_DIR )
  endif()

  if( DEFINED ${NAME}_PREREQUESTS_LIBRARIES )
    list( REMOVE_DUPLICATES ${NAME}_PREREQUESTS_LIBRARIES )
  endif()

endmacro()

# Searches current directory for the sources and headers
macro( make_sources )
  file( GLOB ${PROJECT_NAME}_HEADERS *.hpp )
  file( GLOB ${PROJECT_NAME}_SOURCES *.cpp )

  foreach( SUBDIRECTORY ${${PROJECT_NAME}_SUBDIRECTORIES} )
    file( GLOB SUBDIRECTORY_SOURCES ${SUBDIRECTORY}/*.cpp )
    set( ${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_SOURCES};${SUBDIRECTORY_SOURCES} )
  endforeach()

endmacro()

macro( make_qt_sources )
  make_sources()

 qt4_wrap_cpp( ${PROJECT_NAME}_MOC ${${PROJECT_NAME}_HEADERS} )
 set( ${PROJECT_NAME}_SOURCES "${${PROJECT_NAME}_SOURCES};${${PROJECT_NAME}_MOC}" )
  
endmacro()


macro( make_qt_project )
  make_qt_sources()
  include_directories( ${${PROJECT_NAME}_PREREQUESTS_INCLUDE_DIR} )

  add_executable( ${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} )
  target_link_libraries( ${PROJECT_NAME} ${${PROJECT_NAME}_PREREQUESTS_LIBRARIES} )

  copy_binary( ${PROJECT_NAME} )

  install( TARGETS ${PROJECT_NAME} DESTINATION ${BINARY_PATH} )
endmacro()
