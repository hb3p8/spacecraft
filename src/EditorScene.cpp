#include "EditorScene.hpp"

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


EditorScene::EditorScene( QString modelFileName ) :
  Scene( NULL ),
  m_text( "LMMonoCaps10", 10, Qt::green ),
  m_lastTime( 0 ),
  m_velocity( Vector3f::Zero() ),
  m_isJumping( false ),
  m_justJumped( false ),
  m_blockToInsert( 1 ),
  m_currentOrient( 0 )
{
  m_camera = CameraPtr( new Camera() );

  if( QFile::exists( modelFileName ) )
    m_shipModel.loadFromFile( modelFileName.toStdString(), true );
}

EditorScene::~EditorScene()
{
}

void EditorScene::initialize()
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

    StarBuilder stars;
    stars.buildStarMesh();
    m_starMesh.writeSimpleData( stars.getVertices(), stars.getTexcoords(), stars.getVerticeCount() );
    m_starMesh.attachShader( m_starShader );

    m_shipModel.getMesh().attachShader( m_cubeShader );

    m_cubeShader.release();

    m_camera->setPosition( Eigen::Vector3f( 0.0, 5.5, 2.5 ) );
}

void EditorScene::draw()
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

  if( m_minIntersection.side != SIDE_NO_INTERSECTION )
  {
    //      int selectedBlock = m_shipModel.getBlock( minIntersection.i, minIntersection.j, minIntersection.k );
    m_text.add( "side\t", m_minIntersection.side );
  }

  m_text.add( "block insert type\t", m_blockToInsert );
  m_text.add( "current orientation\t", directionSideTest( m_camera->view() ) );
  m_text.draw( m_widget, 10, 15 );
  m_text.clear();

  m_text.add("+");
  m_text.draw( m_widget, m_widget->width()/2 - 2, m_widget->height()/2 + 3 );
  m_text.clear();
}



void EditorScene::process( int newTime )
{
  int deltaTime = newTime - m_lastTime;
  m_lastTime = newTime;

  float delta = deltaTime / 100.f;

  m_velocity.x() *= 0.7;
  m_velocity.z() *= 0.7;

  applyInput();

  Vector3f downVec( 0.f, -1.f, 0.f );
  m_shipModel.octreeRaycastIntersect( m_camera->position(), downVec, m_moveIntersect );
  Vector3f toGround = downVec * m_moveIntersect.time;
//  Vector3f point = m_camera->position() + toGround;
  if( m_moveIntersect.intersected() )
  {
    float sqrDist = toGround.dot( toGround );
    if( sqrDist > 9.0 + 0.1 ) m_velocity.y() += -0.10;
    else if( sqrDist < 9.0 - 0.1 ) m_velocity.y() = 0.15;
    else
    {
      if( m_justJumped )
        m_justJumped = false;
      else
      {
        m_isJumping = false;
        m_velocity.y() = 0;
      }
    }
  }

  int sideToRowRemap[ 6 ] = { 4, 3, 1, 5, 2, 0 };

  Vector3f viewVec( m_velocity );
  viewVec.y() = 0;
  viewVec.normalize();

  m_shipModel.octreeRaycastIntersect( m_camera->position() + downVec * 2.0, viewVec, m_moveIntersect );
  Vector3f toWall = viewVec * m_moveIntersect.time;
  if( m_moveIntersect.intersected() )
  {
    float sqrDist = toWall.dot( toWall );
    if( sqrDist < 1.5 )
    {
      int index = cubeIndices[ sideToRowRemap[ m_moveIntersect.side ] * 6 ];
      Vector3f wallNormal( cubeNormals[ index ][ 0 ],
                           cubeNormals[ index ][ 1 ],
                           cubeNormals[ index ][ 2 ] );

      // вычитаем из скорости её проекцию на нормаль препятствия
      m_velocity -= wallNormal * ( m_velocity.dot(wallNormal) / wallNormal.dot( wallNormal ) );
    }
  }

  m_camera->translate( m_velocity * delta );
}

void EditorScene::applyInput()
{
  Vector3f delta( 0.0F, 0.0F, 0.0F );
  Vector3f deltaVelocity;
  float scale = 0.7;

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
  deltaVelocity *= scale;

  m_velocity += deltaVelocity;

  // jumping

  i = m_inputMap.find( Qt::Key_Space );
  if( i != m_inputMap.end() && i.value() == true )
  {
    if( !m_isJumping )
    {
      m_velocity.y() += 1.6;
      m_justJumped = true;
      m_isJumping = true;
    }
  }

}

void EditorScene::viewportResize( int w, int h )
{
  m_camera->viewportResize( w, qMax( h, 1 ) );
}

void EditorScene::keyPressEvent( QKeyEvent *e )
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
  case Qt::Key_C:
    if( m_minIntersection.side != SIDE_NO_INTERSECTION )
    {
      size_t offset[3] = { 0, 0, 0 };
      offset[ m_minIntersection.side % 3 ] =  m_minIntersection.side > 2 ? 1 : -1;
      BlockData& block = m_shipModel.getBlock(
            m_minIntersection.i + offset[ 0 ],
            m_minIntersection.j + offset[ 1 ],
            m_minIntersection.k + offset[ 2 ] );
      block.blockType = m_blockToInsert;
      block.orientation = directionSideTest( m_camera->view() );

      m_shipModel.refreshModel();
    }
    break;
  case Qt::Key_X:
    if( m_minIntersection.side != SIDE_NO_INTERSECTION )
    {
      m_shipModel.getBlock(
            m_minIntersection.i,
            m_minIntersection.j,
            m_minIntersection.k ).blockType = 0;
      m_shipModel.refreshModel();
    }
    break;

  case Qt::Key_1:
    m_blockToInsert = 1;
  break;
  case Qt::Key_2:
    m_blockToInsert = 2;
  break;
  case Qt::Key_3:
    m_blockToInsert = 3;
  break;
  case Qt::Key_4:
    m_blockToInsert = 4;
  break;

  case Qt::Key_Plus:
    m_currentOrient += 1;
  break;
  case Qt::Key_Minus:
    m_currentOrient -= 1;
  break;
  }
}

void EditorScene::wheelEvent( QWheelEvent* event )
{
  m_camera->translate( m_camera->rotation() *
                       Vector3f( 0.0, 0.0, static_cast< GLfloat >( event->delta() ) * 1.0 / 100 ) );
}

void EditorScene::mousePressEvent( QMouseEvent* event )
{
  m_lastMousePos = event->pos();
}

void EditorScene::mouseMoveEvent( QMouseEvent* event )
{
  GLfloat dx = static_cast< GLfloat >( event->pos().x() - m_lastMousePos.x() ) / m_widget->width () / 2 * M_PI;
  GLfloat dy = static_cast< GLfloat >( event->pos().y() - m_lastMousePos.y() ) / m_widget->height() / 2 * M_PI;

  if( ( event->buttons() & Qt::LeftButton ) | ( event->buttons() & Qt::RightButton ) )
  {
    m_camera->eyeTurn( dx, dy );
  }

  m_lastMousePos = event->pos();
}

