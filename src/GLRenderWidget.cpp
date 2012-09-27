#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>


using namespace Eigen;


GLRenderWidget::GLRenderWidget( const QGLFormat& format, ScenePtr scene, QWidget* parent ) :
    QGLWidget( format, parent ),
    m_timer( new QTimer( this ) ),
    m_fps( 0.0f ),
    m_frames( 0 ),
    m_refreshTime( 1000 ),
    m_workTime( new QTime() ),
    m_fpsTime( new QTime() ),
    m_scene( scene )
{
  setMinimumSize( defaultXSize, defaultYSize );

//  m_scene = EditorScenePtr( new EditorScene( this, NULL, modelFileName ) );
}

GLRenderWidget::~GLRenderWidget()
{
}

void GLRenderWidget::initializeGL()
{
    assert( m_scene );

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
    m_scene->viewportResize(w,h); //подгонка матрицы проекции под размер окна
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLRenderWidget::keyPressEvent( QKeyEvent* e )
{
  m_scene->getInputMap().insert( e->key(), true );
  m_scene->keyPressEvent( e );
  QGLWidget::keyPressEvent( e );
}

void GLRenderWidget::keyReleaseEvent( QKeyEvent* e )
{
  m_scene->getInputMap().insert( e->key(), false );
  QGLWidget::keyReleaseEvent( e );
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


