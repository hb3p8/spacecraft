#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>


using namespace Eigen;


GLRenderWidget::GLRenderWidget( const QGLFormat& format, QWidget* parent, QString modelFileName ) :
    QGLWidget( format, parent ),
    m_timer( new QTimer( this ) ),
    m_fps( 0.0f ),
    m_frames( 0 ),
    m_refreshTime( 1000 ),
    m_workTime( new QTime() ),
    m_fpsTime( new QTime() )
{
  setMinimumSize( defaultXSize, defaultYSize );

  m_scene = EditorScenePtr( new EditorScene( this, NULL, modelFileName ) );
}

GLRenderWidget::~GLRenderWidget()
{
}

void GLRenderWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    m_timer = new QTimer( this );
    m_timer->start( 10 );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

    m_fpsTime->start();
    m_workTime->start();

    m_scene->initialize();

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Enable blending
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}


void GLRenderWidget::paintGL()
{

  m_scene->draw();
  m_scene->process( m_workTime->elapsed() );

  nextFrame();
}

void GLRenderWidget::resizeGL( int w, int h )
{
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLRenderWidget::keyPressEvent( QKeyEvent* e )
{
  m_scene->keyPressEvent( e );
  switch ( e->key() )
  {
  case Qt::Key_W:
    m_scene->getInputMap().insert( Qt::Key_W, true );
  break;
  case Qt::Key_S:
    m_scene->getInputMap().insert( Qt::Key_S, true );
  break;
  case Qt::Key_A:
    m_scene->getInputMap().insert( Qt::Key_A, true );
  break;
  case Qt::Key_D:
    m_scene->getInputMap().insert( Qt::Key_D, true );
  break;
  case Qt::Key_Space:
    m_scene->getInputMap().insert( Qt::Key_Space, true );
  break;


  default:
    QGLWidget::keyPressEvent( e );
  }
}

void GLRenderWidget::keyReleaseEvent( QKeyEvent* e )
{
  switch ( e->key() )
  {
  case Qt::Key_W:

    m_scene->getInputMap().insert( Qt::Key_W, false );
  break;
  case Qt::Key_S:

    m_scene->getInputMap().insert( Qt::Key_S, false );
  break;
  case Qt::Key_A:

    m_scene->getInputMap().insert( Qt::Key_A, false );
  break;
  case Qt::Key_D:

    m_scene->getInputMap().insert( Qt::Key_D, false );
  break;
  case Qt::Key_Space:

    m_scene->getInputMap().insert( Qt::Key_Space, false );
  break;

  default:
    QGLWidget::keyReleaseEvent( e );
  }
}

void GLRenderWidget::wheelEvent( QWheelEvent* event )
{
  m_scene->wheelEvent( event );
}

void GLRenderWidget::mousePressEvent( QMouseEvent* event )
{
  m_scene->mousePressEvent( event );
}

void GLRenderWidget::mouseMoveEvent( QMouseEvent* event )
{
  m_scene->mouseMoveEvent( event );
}

void GLRenderWidget::nextFrame()
{
  m_frames++;
  int newTime = m_fpsTime->elapsed();

  if( newTime > m_refreshTime )
  {
    m_fps = static_cast< GLfloat >( m_frames ) / newTime * 1000.0;
    m_frames = 0;
    m_fpsTime->restart();
  }
}


