macro( create_directory name )
  file( MAKE_DIRECTORY ${PROJECT_PATH}/bin/${name} )
endmacro()

macro( copy_file name destination )
  get_target_property( FILENAME ${name} LOCATION )
  add_custom_command( TARGET ${name} POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy ${FILENAME} ${destination} )
endmacro()

macro( copy_binary name )
  copy_file( ${name} ${BINARY_PATH}/ )
endmacro()

macro( copy_test name )
  copy_file( ${name} ${BINARY_PATH}/tests/ )
endmacro()

