#include "mainwindow.hpp"

#include <QGLFormat>

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
{  
}

MainWindow::~MainWindow()
{

}

void MainWindow::setRenderWidget( QGLWidget *renderWidget )
{
  m_renderWidget = renderWidget;
  setCentralWidget( m_renderWidget );

  m_renderWidget->setFocus();
}
