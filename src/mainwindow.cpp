#include "mainwindow.hpp"
#include "RenderWidget.hpp"

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
{
  m_renderWidget = new RenderWidget( this );
  m_renderWidget->showFPS( false );
  setCentralWidget( m_renderWidget );
}

MainWindow::~MainWindow()
{
    
}
