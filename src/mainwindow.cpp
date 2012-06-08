#include "mainwindow.hpp"
#include "GLRenderWidget.hpp"

#include <QGLFormat>

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
{  
  QGLFormat glFormat;
  glFormat.setProfile( QGLFormat::CoreProfile );
  glFormat.setSampleBuffers( true );

  m_renderWidget = new GLRenderWidget( glFormat, this );
  setCentralWidget( m_renderWidget );

  m_renderWidget->setFocus();
}

MainWindow::~MainWindow()
{
    
}
