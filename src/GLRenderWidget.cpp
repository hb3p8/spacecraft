#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>
#include <QFile>

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

//GLuint texture;
QMap<std::string, GLuint> textures;

void GLRenderWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    if ( !prepareShaderProgram( m_cubeShader,
                                QString( SPACECRAFT_PATH ) + "/shaders/cube.vert",
                                QString( SPACECRAFT_PATH ) + "/shaders/cube.frag" ) )
      return;

    if ( !prepareShaderProgram( m_starShader,
                                QString( SPACECRAFT_PATH ) + "/shaders/stars.vert",
                                QString( SPACECRAFT_PATH ) + "/shaders/stars.frag" ) )
      return;

    m_timer = new QTimer( this );
    m_timer->start( 10 );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

    QImage starsImage( QString( SPACECRAFT_PATH ) + "/images/stars.jpg" );
    starsImage.setAlphaChannel( QImage( QString( SPACECRAFT_PATH ) + "/images/starsAlpha.jpg" ) );
    textures.insert( "stars", bindTexture( starsImage ) );

    QImage blocksImage( QString( SPACECRAFT_PATH ) + "/images/block_faces.png" );
    textures.insert( "block_faces", bindTexture( blocksImage ) );


    if ( !m_cubeShader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    m_shipModel.refreshModel();

    buildStarMesh();
    m_starMesh.setAttributesToShader( m_starShader );

    m_shipModel.getMesh().setAttributesToShader( m_cubeShader );

    m_cubeShader.release();


    m_fpsTime->start();
    m_workTime->start();

    m_camera->setPosition( Eigen::Vector3f( 0.0, 5.5, 2.5 ) );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Enable blending
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Intersection minIntersection;

int blockToInsert = 1;
int currentOrient = 0;

void GLRenderWidget::paintGL()
{

//  cursor().setPos( width()/2 - 2, height()/2 );


  QVector3D rayPos( eigenVectorToQt( m_camera->position() ) );
  QVector3D rayDir( eigenVectorToQt( m_camera->view() ) );

  m_shipModel.octreeRaycastIntersect( m_camera->position(), m_camera->view(), minIntersection );

  QVector3D point = rayPos + rayDir * minIntersection.time;

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  QMatrix4x4 modelMatrix;
  QMatrix4x4 projectionMatrix( m_camera->projectionMatrix() );
  QMatrix4x4 viewMatrix( m_camera->viewMatrix() );
  QMatrix4x4 viewStarMatrix( viewMatrix );

  // зануляем translation часть в видовой матрице для звёзд
  viewStarMatrix.setColumn( 3, QVector4D( 0.0, 0.0, 0.0, 1.0 ) );

  viewStarMatrix.scale( 10.0 );

  m_starShader.bind();

  glBindTexture( GL_TEXTURE_2D, textures.find( "stars" ).value() );

  glDisable( GL_DEPTH_TEST );

  m_starShader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_starShader.setUniformValue( "viewMatrix", viewStarMatrix );
  m_starShader.setUniformValue( "colorTexture", 0 );

  m_starMesh.draw();
  m_starShader.release();

  glEnable( GL_DEPTH_TEST );

  m_cubeShader.bind();

  glBindTexture( GL_TEXTURE_2D, textures.find( "block_faces" ).value() );

  IndexedMesh& mesh = m_shipModel.getMesh();

  modelMatrix.setToIdentity();
  m_cubeShader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_cubeShader.setUniformValue( "viewMatrix", viewMatrix );
  m_cubeShader.setUniformValue( "modelMatrix", modelMatrix );
  m_cubeShader.setUniformValue( "ambient", 0.8f );
  m_cubeShader.setUniformValue( "baseColor", QVector4D( 0.0, 0.0, 0.0, 0.0 ) );
  m_cubeShader.setUniformValue( "point", point );
  m_cubeShader.setUniformValue( "colorTexture", 0 );


  mesh.draw();

  glBindTexture( GL_TEXTURE_2D, 0 );

  m_cubeShader.release();


  process();

  m_text.add( "camera\t", m_camera->position() );
  m_text.add( "fps\t", m_fps );

  if( minIntersection.side != SIDE_NO_INTERSECTION )
  {
    //      int selectedBlock = m_shipModel.getBlock( minIntersection.i, minIntersection.j, minIntersection.k );
    m_text.add( "side\t", minIntersection.side );
  }

  m_text.add( "block insert type\t", blockToInsert );
  m_text.add( "current orientation\t", directionSideTest( m_camera->view() ) );
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
  if( moveIntersect.intersected() )
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
  if( moveIntersect.intersected() )
  {
    float sqrDist = toWall.dot( toWall );
    if( sqrDist < 1.5 )
    {
      int index = cubeIndices[ sideToRowRemap[ moveIntersect.side ] * 6 ];
      Vector3f wallNormal( cubeNormals[ index ][ 0 ],
                           cubeNormals[ index ][ 1 ],
                           cubeNormals[ index ][ 2 ] );

      // вычитаем из скорости её проекцию на нормаль препятствия
      velocity -= wallNormal * ( velocity.dot(wallNormal) / wallNormal.dot( wallNormal ) );
    }
  }

  m_camera->translate( velocity );
}

void GLRenderWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
    m_camera->viewportResize( w, qMax( h, 1 ) );
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
      velocity.y() += 0.35;
      justJumped = true;
      isJumping = true;
    }
  }

}

const int STAR_COUNT = 800;
const float PATCH_SIZE = 0.8;

void GLRenderWidget::buildStarMesh()
{
  float* vertices;
  float* normals;
  float* texcoords;

  size_t verticesSize = STAR_COUNT * 6 * 3;
  size_t texcoordsSize = STAR_COUNT * 6 * 2;

  vertices = new float[ verticesSize ];
  normals = new float[ verticesSize ];
  texcoords = new float[ texcoordsSize ];

  std::vector< Vector2f > quadTex;
  quadTex.push_back( Vector2f( 0.0, 1.0 ) );
  quadTex.push_back( Vector2f( 1.0, 1.0 ) );
  quadTex.push_back( Vector2f( 1.0, 0.0 ) );
  quadTex.push_back( Vector2f( 0.0, 0.0 ) );


  int indices[] = { 0, 3, 1, 1, 3, 2 };
  int vertCounter = 0;

  int ranseq = 123;
  for ( int s = 0; s < STAR_COUNT; s++ )
  {
    // get a random point, normalize for center of patch
    Vector3f eye;

    int p = (ranseq++) * 17;
    p = (p<<13) ^ p;
    eye.x() = 1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
    eye.y() = 1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0;
    eye.z() = 1.0 - ((p * (p * p * 23431 + 830237) + 1376312589) & 0x7fffffff) / 1073741824.0;
    eye.normalize();

    // get a second random point
    Vector3f xaxis;

    p = (ranseq++) * 17;
    p = (p<<13) ^ p;
    xaxis.x() = 1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
    xaxis.y() = 1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0;
    xaxis.z() = 1.0 - ((p * (p * p * 23431 + 830237) + 1376312589) & 0x7fffffff) / 1073741824.0;

    // cross with eye to get two axis
    xaxis = xaxis.cross(eye);
    xaxis.normalize();
    Vector3f yaxis(xaxis);
    yaxis = yaxis.cross(eye);

    // randomly flip the axis to cut down on visible repetition
    p = (ranseq++) * 17;
    p = (p<<13) ^ p;
    bool xflip = 0 < (1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
    bool yflip = 0 < (1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0);

    // build corner points
    Vector3f normal(eye);
    normal *= -1;
    xaxis *= PATCH_SIZE;
    if (xflip)
      xaxis *= -1;
    yaxis *= PATCH_SIZE;
    if (yflip)
      yaxis *= -1;
    eye *= 0.49;

    std::vector< Vector3f > quadVerts;
    quadVerts.push_back( eye );
    quadVerts[ quadVerts.size() - 1 ] -= xaxis;
    quadVerts[ quadVerts.size() - 1 ] += yaxis;

    quadVerts.push_back( eye );
    quadVerts[ quadVerts.size() - 1 ] += xaxis;
    quadVerts[ quadVerts.size() - 1 ] += yaxis;

    quadVerts.push_back( eye );
    quadVerts[ quadVerts.size() - 1 ] += xaxis;
    quadVerts[ quadVerts.size() - 1 ] -= yaxis;

    quadVerts.push_back( eye );
    quadVerts[ quadVerts.size() - 1 ] -= xaxis;
    quadVerts[ quadVerts.size() - 1 ] -= yaxis;


    for( int i = 0; i < 6; i++ )
    {
      int idx = indices[ i ];
      float* vertex = vertices + vertCounter * 3;
      Vector3f current = quadVerts[ idx ] + eye * 10;
      vertex[ 0 ] = current.x();
      vertex[ 1 ] = current.y();
      vertex[ 2 ] = current.z();

      float* norm = normals + vertCounter * 3;
      norm[ 0 ] = normal.x();
      norm[ 1 ] = normal.y();
      norm[ 2 ] = normal.z();

      float* tex = texcoords + vertCounter * 2;
      tex[ 0 ] = quadTex[ idx ].x();
      tex[ 1 ] = quadTex[ idx ].y();

      vertCounter++;
    }

  }

  m_starMesh.allocateBuffers( vertices, normals, texcoords, vertCounter );

//  for( int i = 0; i < vertCounter * 3; i++ )
//  {
//    if( i % 3 == 0 ) std::cout << std::endl;
//    std::cout << vertices[ i ] << std::endl;
//  }


  delete[] vertices;
  delete[] normals;
  delete[] texcoords;

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
      BlockData& block = m_shipModel.getBlock(
                            minIntersection.i + offset[ 0 ],
                            minIntersection.j + offset[ 1 ],
                            minIntersection.k + offset[ 2 ] );
      block.blockType = blockToInsert;
      block.orientation = directionSideTest( m_camera->view() );

      m_shipModel.refreshModel();
    }
  break;
  case Qt::Key_X:
    if( minIntersection.side != SIDE_NO_INTERSECTION )
    {
      m_shipModel.getBlock(
            minIntersection.i,
            minIntersection.j,
            minIntersection.k ).blockType = 0;
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

  case Qt::Key_1:
    blockToInsert = 1;
  break;
  case Qt::Key_2:
    blockToInsert = 2;
  break;
  case Qt::Key_3:
    blockToInsert = 3;
  break;
  case Qt::Key_4:
    blockToInsert = 4;
  break;

  case Qt::Key_Plus:
    currentOrient += 1;
  break;
  case Qt::Key_Minus:
    currentOrient -= 1;
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

    m_inputMap.insert( Qt::Key_W, false );
  break;
  case Qt::Key_S:

    m_inputMap.insert( Qt::Key_S, false );
  break;
  case Qt::Key_A:

    m_inputMap.insert( Qt::Key_A, false );
  break;
  case Qt::Key_D:

    m_inputMap.insert( Qt::Key_D, false );
  break;
  case Qt::Key_Space:

    m_inputMap.insert( Qt::Key_Space, false );
  break;

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


