#include "mainwindow.hpp"
#include "RenderWidget.hpp"

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
{
  m_renderWidget = new RenderWidget( this );
  setCentralWidget( m_renderWidget );
}

MainWindow::~MainWindow()
{
    
}
