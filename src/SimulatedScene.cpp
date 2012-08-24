#include "SimulatedScene.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>
#include <QFile>

#include "Utils.hpp"
#include "GLRenderWidget.hpp"
#include "MeshData/Stars.hpp"

#include <memory>
#include <math.h>

using namespace Eigen;


SimulatedScene::SimulatedScene( QString modelFileName ) :
  Scene( NULL ),
  m_text( "LMMonoCaps10", 10, Qt::green ),
  m_lastTime( 0 ),
  m_velocity( Vector3f::Zero() )
{
  m_camera = CameraPtr( new Camera() );

  if( QFile::exists( modelFileName ) )
    m_shipModel.loadFromFile( modelFileName.toStdString(), true );
}

SimulatedScene::~SimulatedScene()
{
}

void SimulatedScene::initialize()
{
    assert( m_widget );

    if ( !prepareShaderProgram( m_cubeShader,
                                QString( SPACECRAFT_PATH ) + "/shaders/cube.vert",
                                QString( SPACECRAFT_PATH ) + "/shaders/cube.frag" ) )
      return;

    if ( !prepareShaderProgram( m_starShader,
                                QString( SPACECRAFT_PATH ) + "/shaders/stars.vert",
                                QString( SPACECRAFT_PATH ) + "/shaders/stars.frag" ) )
      return;

    QImage starsImage( QString( SPACECRAFT_PATH ) + "/images/stars.jpg" );
    starsImage.setAlphaChannel( QImage( QString( SPACECRAFT_PATH ) + "/images/starsAlpha.jpg" ) );
    m_textures.insert( "stars", m_widget->bindTexture( starsImage ) );

    QImage blocksImage( QString( SPACECRAFT_PATH ) + "/images/block_faces.png" );
    m_textures.insert( "block_faces", m_widget->bindTexture( blocksImage ) );


    if ( !m_cubeShader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    m_shipModel.refreshModel();
    m_shipModel.calculateMassCenter();

    StarBuilder stars;
    stars.buildStarMesh();
    m_starMesh.writeSimpleData( stars.getVertices(), stars.getTexcoords(), stars.getVerticeCount() );
    m_starMesh.attachShader( m_starShader );

    m_shipModel.getMesh().attachShader( m_cubeShader );

    m_cubeShader.release();

    m_camera->setPosition( Eigen::Vector3f( 0.0, 0.0, 0.0 ) );

    m_shipModel.findEngines();
}

bool engineRunning = false;

void SimulatedScene::draw()
{

  QVector3D rayPos( eigenVectorToQt( m_camera->position() ) );
  QVector3D rayDir( eigenVectorToQt( m_camera->view() ) );

  m_shipModel.octreeRaycastIntersect( m_camera->position(), m_camera->view(), m_minIntersection );

  QVector3D point = rayPos + rayDir * m_minIntersection.time;

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  QMatrix4x4 modelMatrix;
  QMatrix4x4 projectionMatrix( m_camera->projectionMatrix() );
  QMatrix4x4 viewMatrix( m_camera->viewMatrix() );
  QMatrix4x4 viewStarMatrix( viewMatrix );

  // зануляем translation часть в видовой матрице для звёзд
  viewStarMatrix.setColumn( 3, QVector4D( 0.0, 0.0, 0.0, 1.0 ) );

  viewStarMatrix.scale( 10.0 );

  m_starShader.bind();

  glBindTexture( GL_TEXTURE_2D, m_textures.find( "stars" ).value() );

  glDisable( GL_DEPTH_TEST );

  m_starShader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_starShader.setUniformValue( "viewMatrix", viewStarMatrix );
  m_starShader.setUniformValue( "colorTexture", 0 );

  m_starMesh.drawSimple();
  m_starShader.release();

  glEnable( GL_DEPTH_TEST );

  m_cubeShader.bind();

  glBindTexture( GL_TEXTURE_2D, m_textures.find( "block_faces" ).value() );

  Mesh& mesh = m_shipModel.getMesh();


  modelMatrix.setToIdentity();
  modelMatrix.translate( eigenVectorToQt( m_shipModel.shipPosition() ) );

  modelMatrix.rotate( m_shipModel.shipRotation().angle() / M_PI * 180.,
                      eigenVectorToQt( m_shipModel.shipRotation().axis() ) );

  modelMatrix.translate( eigenVectorToQt( m_shipModel.getMassCenter() * (-1) ) );

  m_cubeShader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_cubeShader.setUniformValue( "viewMatrix", viewMatrix );
  m_cubeShader.setUniformValue( "modelMatrix", modelMatrix );
  m_cubeShader.setUniformValue( "ambient", 0.8f );
  m_cubeShader.setUniformValue( "baseColor", QVector4D( 0.0, 0.0, 0.0, 0.0 ) );
  m_cubeShader.setUniformValue( "point", point );
  m_cubeShader.setUniformValue( "colorTexture", 0 );


  mesh.drawIndexed();

  glBindTexture( GL_TEXTURE_2D, 0 );

  m_cubeShader.release();

  m_text.add( "fps\t", dynamic_cast< GLRenderWidget* >( m_widget )->getFPS() );
  m_text.add( "camera\t", m_camera->position() );

  m_text.add( "engine run\t", engineRunning );

  m_text.add( "mass center\t", m_shipModel.getMassCenter() );
  m_text.draw( m_widget, 10, 15 );
  m_text.clear();

  m_text.add("+");
  m_text.draw( m_widget, m_widget->width()/2 - 2, m_widget->height()/2 + 3 );
  m_text.clear();
}



void SimulatedScene::process( int newTime )
{
  int deltaTime = newTime - m_lastTime;
  m_lastTime = newTime;

  float delta = deltaTime / 100.f;

  m_velocity *= 0.7;

  applyInput();

  m_camera->translate( m_velocity * delta );

  m_shipModel.process( delta );
}

void SimulatedScene::applyInput()
{
  Vector3f delta( 0.0F, 0.0F, 0.0F );
  Vector3f deltaVelocity;
  float scale = 1.0;

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

  m_velocity += deltaVelocity * scale;

}

void SimulatedScene::viewportResize( int w, int h )
{
  m_camera->viewportResize( w, qMax( h, 1 ) );
}

void SimulatedScene::keyPressEvent( QKeyEvent *e )
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

  case Qt::Key_M:
    m_camera->setPosition( m_shipModel.getMassCenter() );
  break;

  case Qt::Key_E:
    engineRunning = !engineRunning;
    if( engineRunning )
    {
      for( BlockRef engine : m_shipModel.getEngines() )
      {
        int idx = engine.generalIndex( m_shipModel.modelSize() );
        m_shipModel.enginePower().insert( idx, 1.0 );
      }

    } else
    {
      for( BlockRef engine : m_shipModel.getEngines() )
        m_shipModel.enginePower().insert( engine.generalIndex( m_shipModel.modelSize() ), 0.0 );
    }
  break;

  }
}

void SimulatedScene::wheelEvent( QWheelEvent* event )
{
  m_camera->translate( m_camera->rotation() *
                       Vector3f( 0.0, 0.0, static_cast< GLfloat >( event->delta() ) * 1.0 / 100 ) );
}

void SimulatedScene::mousePressEvent( QMouseEvent* event )
{
  m_lastMousePos = event->pos();
}

void SimulatedScene::mouseMoveEvent( QMouseEvent* event )
{
  GLfloat dx = static_cast< GLfloat >( event->pos().x() - m_lastMousePos.x() ) / m_widget->width () / 2 * M_PI;
  GLfloat dy = static_cast< GLfloat >( event->pos().y() - m_lastMousePos.y() ) / m_widget->height() / 2 * M_PI;

  if( ( event->buttons() & Qt::LeftButton ) | ( event->buttons() & Qt::RightButton ) )
  {
    m_camera->eyeTurn( dx, dy );
  }

  m_lastMousePos = event->pos();
}

