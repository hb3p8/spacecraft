macro( ConfigurePackage NAME )

if( NOT ${NAME}_PREREQUESTS_FOUND 
    OR ${${NAME}_PRIMARY_INCLUDE_DIR} STREQUAL ${NAME}_PRIMARY_INCLUDE_DIR-NOTFOUND)
  set( ${NAME}_FOUND FALSE )

  if( ${NAME}_FIND_REQUIRED )
    message( FATAL_ERROR "Cannot find ${NAME}" )
  elseif( NOT ${NAME}_FIND_QUIETLY )
    message( WARNING "Cannot find ${NAME}" )
  endif()
else()
  set( ${NAME}_FOUND TRUE )
endif()

set( ${NAME}_INCLUDE_DIR "${${NAME}_PRIMARY_INCLUDE_DIR};${${NAME}_PREREQUESTS_INCLUDE_DIR}" )

endmacro()