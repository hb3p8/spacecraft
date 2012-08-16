#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>
#include <QFile>

#include "Mesh.hpp"
#include "Utils.hpp"

#include <memory>
#include <math.h>

using namespace Eigen;


GLRenderWidget::GLRenderWidget( const QGLFormat& format, QWidget* parent ) :
    QGLWidget( format, parent ),
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

  if( QFile::exists( "default.txt" ) )
    m_shipModel.loadFromFile( "default.txt" );
}

GLRenderWidget::~GLRenderWidget()
{

}

GLuint texture;

void GLRenderWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );



    // Prepare a complete shader program...
    if ( !prepareShaderProgram( m_shader, QString( SPACECRAFT_PATH ) + "/shaders/vert.glsl",
                                QString( SPACECRAFT_PATH ) + "/shaders/frag.glsl" ) )
        return;

    m_timer = new QTimer( this );
    m_timer->start( 10 );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

    QImage texImage( QString( SPACECRAFT_PATH ) + "/images/default.png" );
    texture = bindTexture( texImage );


    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    m_shipModel.refreshModel();

    m_shipModel.getMesh().setAttributesToShader( m_shader );

    m_shader.release();


    m_fpsTime->start();
    m_workTime->start();

    m_camera->setPosition( Eigen::Vector3f( 0.0, 5.0, 2.0 ) );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

}

Intersection minIntersection;

void GLRenderWidget::paintGL()
{
    QVector3D rayPos( eigenVectorToQt( m_camera->position() ) );
    QVector3D rayDir( eigenVectorToQt( m_camera->view() ) );

    m_shipModel.octreeRaycastIntersect( m_camera->position(), m_camera->view(), minIntersection );

    QVector3D point = rayPos + rayDir * minIntersection.time;

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QMatrix4x4 modelMatrix;
    QMatrix4x4 projectionMatrix( m_camera->projectionMatrix() );
    QMatrix4x4 viewMatrix( m_camera->viewMatrix() );

    m_shader.bind();

    // Draw stuff

    glBindTexture( GL_TEXTURE_2D, texture );

    Mesh& mesh = m_shipModel.getMesh();

    modelMatrix.setToIdentity();
    m_shader.setUniformValue( "projectionMatrix", projectionMatrix );
    m_shader.setUniformValue( "viewMatrix", viewMatrix );
    m_shader.setUniformValue( "modelMatrix", modelMatrix );
    m_shader.setUniformValue( "ambient", 0.8f );
    m_shader.setUniformValue( "baseColor", QVector4D( 0.0, 0.0, 0.0, 0.0 ) );
    m_shader.setUniformValue( "point", point );
    m_shader.setUniformValue( "colorTexture", 0 );


    mesh.draw();

    glBindTexture( GL_TEXTURE_2D, 0 );

    m_shader.release();    


    process();

    m_text.add( "camera\t", m_camera->position() );
    m_text.add( "fps\t", m_fps );

    if( minIntersection.side != SIDE_NO_INTERSECTION )
    {
//      int selectedBlock = m_shipModel.getBlock( minIntersection.i, minIntersection.j, minIntersection.k );
      m_text.add( "side\t", minIntersection.side );
    }

    m_text.draw( this, 10, 15 );
    m_text.clear();

    m_text.add("+");
    m_text.draw( this, width()/2 - 2, height()/2 + 3 );
    m_text.clear();

    nextFrame();
}

Vector3f velocity;
bool isJumping = false;
bool justJumped = false;
Intersection moveIntersect;

void GLRenderWidget::process()
{
  velocity.x() *= 0.6;
  velocity.z() *= 0.6;

  applyInput();

  Vector3f downVec( 0.f, -1.f, 0.f );
  m_shipModel.octreeRaycastIntersect( m_camera->position(), downVec, moveIntersect );
  Vector3f toGround = downVec * moveIntersect.time;
//  Vector3f point = m_camera->position() + toGround;
  if( minIntersection.intersected() )
  {
    float sqrDist = toGround.dot( toGround );
    if( sqrDist > 9.0 + 0.1 ) velocity.y() += -0.02;
    else if( sqrDist < 9.0 - 0.1 ) velocity.y() = 0.03;
    else
    {
      if( justJumped )
        justJumped = false;
      else
      {
        isJumping = false;
        velocity.y() = 0;
      }
    }
  }

  int sideToRowRemap[ 6 ] = { 4, 3, 1, 5, 2, 0 };

  Vector3f viewVec( velocity );
  viewVec.y() = 0;
  viewVec.normalize();

  m_shipModel.octreeRaycastIntersect( m_camera->position() + downVec * 2.0, viewVec, moveIntersect );
  Vector3f toWall = viewVec * moveIntersect.time;
  if( minIntersection.intersected() )
  {
    float sqrDist = toWall.dot( toWall );
    if( sqrDist < 1.5 )
    {
      int index = cubeIndices[ sideToRowRemap[ moveIntersect.side ] * 6 ];
      Vector3f wallNormal( cubeNormals[ index ][ 0 ],
                           cubeNormals[ index ][ 1 ],
                           cubeNormals[ index ][ 2 ] );

      velocity -= wallNormal * ( velocity.dot(wallNormal) / wallNormal.dot( wallNormal ) );
    }
  }

  m_camera->translate( velocity );
}

void GLRenderWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void GLRenderWidget::applyInput()
{
  Vector3f delta( 0.0F, 0.0F, 0.0F );
  Vector3f deltaVelocity;
  float step = 0.15;

  InputMap::const_iterator i;

  i = m_inputMap.find( Qt::Key_W );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 2 ] += 1.0;

  i = m_inputMap.find( Qt::Key_S );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 2 ] -= 1.0;

  i = m_inputMap.find( Qt::Key_A );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 0 ] += 1.0;

  i = m_inputMap.find( Qt::Key_D );
  if( i != m_inputMap.end() && i.value() == true )
    delta[ 0 ] -= 1.0;

  deltaVelocity = m_camera->rotation() * delta;
  deltaVelocity.y() = 0;
  if( deltaVelocity.dot( deltaVelocity ) > 1e-6 )
    deltaVelocity.normalize();
  deltaVelocity *= step;

  velocity += deltaVelocity;

  // jumping

  i = m_inputMap.find( Qt::Key_Space );
  if( i != m_inputMap.end() && i.value() == true )
  {
    if( !isJumping )
    {
      velocity.y() += 0.5;
      justJumped = true;
      isJumping = true;
    }
  }

}

void GLRenderWidget::keyPressEvent( QKeyEvent* e )
{
  static bool wireframeMode = false;
  switch ( e->key() )
  {
  case Qt::Key_Escape:
    QCoreApplication::instance()->quit();
    m_shipModel.saveToFile( "default.txt" );
    break;
  case Qt::Key_F:
    if( wireframeMode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    else
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    wireframeMode = !wireframeMode;
    break;
  case Qt::Key_Space:
    m_inputMap.insert( Qt::Key_Space, true );
    break;
  case Qt::Key_C:
    if( minIntersection.side != SIDE_NO_INTERSECTION )
    {
      size_t offset[3] = { 0, 0, 0 };
      offset[ minIntersection.side % 3 ] =  minIntersection.side > 2 ? 1 : -1;
      m_shipModel.getBlock(
            minIntersection.i + offset[ 0 ],
            minIntersection.j + offset[ 1 ],
            minIntersection.k + offset[ 2 ] ) = 1;
      m_shipModel.refreshModel();
    }
  break;
  case Qt::Key_X:
    if( minIntersection.side != SIDE_NO_INTERSECTION )
    {
      m_shipModel.getBlock(
            minIntersection.i,
            minIntersection.j,
            minIntersection.k ) = 0;
      m_shipModel.refreshModel();
    }
 break;
  case Qt::Key_W:

    m_inputMap.insert( Qt::Key_W, true );
  break;
  case Qt::Key_S:

    m_inputMap.insert( Qt::Key_S, true );
  break;
  case Qt::Key_A:

    m_inputMap.insert( Qt::Key_A, true );
  break;
  case Qt::Key_D:

    m_inputMap.insert( Qt::Key_D, true );
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
  case Qt::Key_Space:
  {
    m_inputMap.insert( Qt::Key_Space, false );
    break;
  }

  default:
    QGLWidget::keyReleaseEvent( e );
  }
}

void GLRenderWidget::wheelEvent( QWheelEvent* event )
{
  m_camera->translate( m_camera->rotation() *
                       Vector3f( 0.0, 0.0, static_cast< GLfloat >( event->delta() ) * 1.0 / 100 ) );
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
    m_camera->eyeTurn( dx, dy );
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
