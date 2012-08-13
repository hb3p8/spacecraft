project( Game )

set( Game_QT_LIBRARIES QtOpenGL )
find_prerequests( Game REQUIRED Qt4 Eigen )

configure_project()

add_definitions ( -DSPACECRAFT_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\" )

make_qt_project()
