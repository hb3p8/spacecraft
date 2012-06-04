#include <QAction>
#include <QKeyEvent>
#include <QColorDialog>

#include "BaseRenderWidget.hpp"

using Eigen::Vector3f;
using Eigen::AngleAxisf;

BaseRenderWidget::BaseRenderWidget( QWidget *parent ) : QGLWidget( parent ), _showFPS  ( true ),
							_timer( new QTimer( this )),
							_fps( 0.0F ),
							_frames( 0 ),
							_refreshTime( 1000 ),
							_workTime( new QTime() ), 
							_fpsTime( new QTime()), 
                            _text( "LMMonoCaps10", 10, Qt::green )
{
  setFormat( QGLFormat( QGL::DoubleBuffer | QGL::DepthBuffer ) );
  setAutoBufferSwap( false );
 
  _timer = new QTimer( this );

  _timer->start( 10 );
  
  connect( _timer, SIGNAL( timeout()), this, SLOT( updateWindow() ) ); 
  
}

BaseRenderWidget::~BaseRenderWidget()
{
}


void BaseRenderWidget::stopRendering()
{
  _timer->stop();
}

void BaseRenderWidget::startRendering()
{
  _timer->start();
}

void BaseRenderWidget::initializeGL()
{
  setMinimumSize( defaultXSize, defaultYSize );
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glOrtho ( -1.0F, 1.0F, -1.0F, 1.0F, -1.0F, 1.0F );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  qglClearColor( Qt::black );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_CULL_FACE  );
  
  _fpsTime->start(); 
  _workTime->start(); 
}

void BaseRenderWidget::resizeGL( int width, int height )
{
  glViewport( 0, 0, width, height );
}

void BaseRenderWidget::paintGL()
{ 
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  draw( static_cast< double >( _workTime->elapsed() ) / 1000 );

  if( _showFPS )
    _text.add( "FPS\t", _fps );

  _text.draw( this, 10, 15 );
  _text.clear();

  swapBuffers();
  nextFrame();
}

void BaseRenderWidget::updateWindow()
{
  updateGL();
}

void BaseRenderWidget::showFPS( bool use )
{
  _showFPS = use;
}

GLfloat BaseRenderWidget::fps() const
{
  return _fps;
}

void BaseRenderWidget::draw( double )
{
}

void BaseRenderWidget::nextFrame()
{
  _frames++;
  int newTime = _fpsTime->elapsed();
  
  if( newTime > _refreshTime )
  {
    _fps = static_cast< GLfloat >( _frames ) / newTime * 1000.0;
    _frames = 0;
    _fpsTime->restart();
  }
}
