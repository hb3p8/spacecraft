#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>

#include "Mesh.hpp"
#include "Utils.hpp"

#include <memory>
#include <math.h>

using namespace Eigen;


GLRenderWidget::GLRenderWidget( const QGLFormat& format, QWidget* parent ) :
    QGLWidget( format, parent ),
    m_vertexBuffer( QGLBuffer::VertexBuffer ),
    m_normalBuffer( QGLBuffer::VertexBuffer ),
    m_texcoordBuffer( QGLBuffer::VertexBuffer ),
    m_indexBuffer( QGLBuffer::IndexBuffer ),
    m_timer( new QTimer( this ) ),
    m_fps( 0.0f ),
    m_frames( 0 ),
    m_refreshTime( 1000 ),
    m_workTime( new QTime() ),
    m_fpsTime( new QTime() ),
    m_text( "LMMonoCaps10", 10, Qt::green )
{
  setMinimumSize( defaultXSize, defaultYSize );

  m_camera = CameraPtr( new Camera() );
}

GLRenderWidget::~GLRenderWidget()
{

}

GLuint texture;
#define SIZE 32
char boxes[SIZE][SIZE][SIZE];

Mesh* cubeMesh;

void GLRenderWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Prepare a complete shader program...
    if ( !prepareShaderProgram( m_shader, "shaders/vert.glsl", "shaders/frag.glsl" ) )
        return;

    m_timer = new QTimer( this );
    m_timer->start( 10 );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

    QImage texImage( "images/default.png" );
    texture = bindTexture( texImage );

    cubeMesh = new Mesh();

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    cubeMesh->setAttributesToShader( m_shader );

    m_shader.release();


    m_fpsTime->start();
    m_workTime->start();

    m_camera->setPosition( Eigen::Vector3f( 0.0, 0.0, -6.0 ) );

    for( size_t i = 0; i < SIZE; i++ )
      for( size_t j = 0; j < SIZE; j++ )
        for( size_t k = 0; k < SIZE; k++ )
        {
          if( j == 0||j==1||j==2||j==3||j==4||j==5||j==6||j==7||j==8||j==9 /*rand() % 10 > 6 */ )
            boxes[ i ][ j ][ k ] = 1;
          else
            boxes[ i ][ j ][ k ] = 0;
        }


    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

}

struct Intersection
{
  size_t i, j, k;
  float time;
  size_t side;
};

Intersection minIntersection;

void GLRenderWidget::paintGL()
{
    float iTime;
    size_t side;

    QVector3D rayPos( m_camera->position().x(),
                      m_camera->position().y(),
                      m_camera->position().z() );

    QVector3D rayDir( m_camera->view().x(),
                      m_camera->view().y(),
                      m_camera->view().z() );

    minIntersection.time = 1e+10;
    minIntersection.side = SIDE_NO_INTERSECTION;

    for( size_t i = 0; i < SIZE; i++ )
      for( size_t j = 0; j < SIZE; j++ )
        for( size_t k = 0; k < SIZE; k++ )
        {
          Vector3f min( 2. * i - 1., 2. * j - 1., 2. * k - 1.);
          Vector3f max( 2. * i + 1., 2. * j + 1., 2. * k + 1.);

          if( boxes[ i ][ j ][ k ] == 1 )
            if( rayBoxIntersection( Vector3f( rayPos.x(), rayPos.y(), rayPos.z() ),
                                    Vector3f( rayDir.x(), rayDir.y(), rayDir.z() ),
                                    min,
                                    max,
                                    &iTime, &side ) )
            {
              if( iTime < minIntersection.time )
              {
                minIntersection.i = i;
                minIntersection.j = j;
                minIntersection.k = k;
                minIntersection.time = iTime;
                minIntersection.side = side;
              }
            }

        }




    QVector3D point = rayPos + rayDir * minIntersection.time;


    applyInput();

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QMatrix4x4 modelMatrix;
    QMatrix4x4 projectionMatrix( m_camera->projectionMatrix() );
    QMatrix4x4 viewMatrix( m_camera->viewMatrix() );

    m_shader.bind();

    m_shader.setUniformValue( "colorTexture", 0 );

    m_shader.enableAttributeArray( "position" );
    m_shader.enableAttributeArray( "normal" );
    m_shader.enableAttributeArray( "texcoord" );

    // Draw stuff

    glBindTexture( GL_TEXTURE_2D, texture );

    for( size_t i = 0; i < SIZE; i++ )
      for( size_t j = 0; j < SIZE; j++ )
        for( size_t k = 0; k < SIZE; k++ )
        {
          if( boxes[ i ][ j ][ k ] == 1 )
          {
            Vector3f translation( 2.0 * i, 2.0 * j, 2.0 * k );

            modelMatrix.setToIdentity();
            modelMatrix.translate( translation.x(), translation.y(), translation.z() );
            m_shader.setUniformValue( "projectionMatrix", projectionMatrix );
            m_shader.setUniformValue( "viewMatrix", viewMatrix );
            m_shader.setUniformValue( "modelMatrix", modelMatrix );

            if( minIntersection.side != SIDE_NO_INTERSECTION )
            {
              m_shader.setUniformValue( "point", point );

              if( minIntersection.i == i && minIntersection.j == j && minIntersection.k == k )
              {
                m_shader.setUniformValue( "baseColor", QVector4D( 0.5, 0.0, 0.0, 0.0 ) );
              }
              else
              {
                m_shader.setUniformValue( "baseColor", QVector4D( 0.0, 0.0, 0.0, 0.0 ) );
              }
            }
            cubeMesh->draw();
          }
        }

    /*m_shader.setUniformValue( "projectionMatrix", projectionMatrix );
    m_shader.setUniformValue( "viewMatrix", viewMatrix );
    m_shader.setUniformValue( "modelMatrix", modelMatrix );

    cubeMesh->draw();*/


    glBindTexture( GL_TEXTURE_2D, 0 );

    m_shader.disableAttributeArray( "position" );
    m_shader.disableAttributeArray( "normal" );
    m_shader.disableAttributeArray( "texcoord" );

    m_shader.release();



    m_text.add( "CAMERA\t", m_camera->position() );
    m_text.add( "FPS\t", m_fps );
    m_text.draw( this, 10, 15 );
    m_text.clear();

    m_text.add("+");
    m_text.draw( this, width()/2 - 2, height()/2 + 3 );
    m_text.clear();

    nextFrame();
}

void GLRenderWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLRenderWidget::applyInput()
{
  Vector3f delta( 0.0F, 0.0F, 0.0F );
  float step = 2.2;

  InputMap::const_iterator i;

  i = m_inputMap.find( Qt::Key_W );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 2 ] += 0.1 * step;

  i = m_inputMap.find( Qt::Key_S );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 2 ] -= 0.1 * step;

  i = m_inputMap.find( Qt::Key_A );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 0 ] += 0.1 * step;

  i = m_inputMap.find( Qt::Key_D );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 0 ] -= 0.1 * step;

  m_camera->translate( delta );
}

void GLRenderWidget::keyPressEvent( QKeyEvent* e )
{
    static bool wireframeMode = false;
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;
        case Qt::Key_F:
            if( wireframeMode )
              glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            else
              glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            wireframeMode = !wireframeMode;
            break;
        case Qt::Key_Space:
        {
          size_t offset[3] = { 0, 0, 0 };
          offset[ minIntersection.side % 3 ] =  minIntersection.side > 2 ? 1 : -1;
          boxes[ minIntersection.i + offset[ 0 ] ]
               [ minIntersection.j + offset[ 1 ] ]
               [ minIntersection.k + offset[ 2 ] ] = 1;
        }break;
        case Qt::Key_W:
        {
          m_inputMap.insert( Qt::Key_W, true );
        }break;
        case Qt::Key_S:
        {
          m_inputMap.insert( Qt::Key_S, true );
        }break;
        case Qt::Key_A:
        {
          m_inputMap.insert( Qt::Key_A, true );
        }break;
        case Qt::Key_D:
        {
          m_inputMap.insert( Qt::Key_D, true );
        }break;
  /*      case Qt::Key_Q:
        {
          m_camera->rotate( AngleAxisf( 0.0, 0.0, -5.0 / 360 * M_PI ));
          return;
        }break;
        case Qt::Key_E:
        {
          m_camera->rotate( Vector3f( 0.0, 0.0, 5.0 / 360 * M_PI ));
          return;
        }break;*/

        default:
            QGLWidget::keyPressEvent( e );
    }
}

void GLRenderWidget::keyReleaseEvent( QKeyEvent* e )
{
  switch ( e->key() )
  {
    case Qt::Key_W:
    {
      m_inputMap.insert( Qt::Key_W, false );
    }break;
    case Qt::Key_S:
    {
      m_inputMap.insert( Qt::Key_S, false );
    }break;
    case Qt::Key_A:
    {
      m_inputMap.insert( Qt::Key_A, false );
    }break;
    case Qt::Key_D:
    {
      m_inputMap.insert( Qt::Key_D, false );
    }break;

    default:
        QGLWidget::keyReleaseEvent( e );
  }
}

void GLRenderWidget::wheelEvent( QWheelEvent* event )
{
  m_camera->translate( Vector3f( 0.0, 0.0, static_cast< GLfloat >( event->delta() ) * 1.0 / 100 ) );
}

void GLRenderWidget::mousePressEvent( QMouseEvent* event )
{
  m_lastMousePos = event->pos();
}

void GLRenderWidget::mouseMoveEvent( QMouseEvent* event )
{
  GLfloat dx = static_cast< GLfloat >( event->pos().x() - m_lastMousePos.x() ) / width () / 2 * M_PI;
  GLfloat dy = static_cast< GLfloat >( event->pos().y() - m_lastMousePos.y() ) / height() / 2 * M_PI;

  if( ( event->buttons() & Qt::LeftButton ) | ( event->buttons() & Qt::RightButton ) )
  {
    m_camera->rotate( AngleAxisf( -dx, Vector3f( 0.0, 1.0, 0.0 ) ) );
    m_camera->rotate( AngleAxisf(  dy, Vector3f( 1.0, 0.0, 0.0 ) ) );
  }

  m_lastMousePos = event->pos();
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
