project( Game )

set( Game_QT_LIBRARIES QtOpenGL )
find_prerequests( Game REQUIRED Qt4 GLOG Eigen GLEW )
configure_project()
make_qt_project()
