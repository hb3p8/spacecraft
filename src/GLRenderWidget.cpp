#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>

#include <memory>

using namespace Eigen;


GLRenderWidget::GLRenderWidget( const QGLFormat& format, QWidget* parent ) :
    QGLWidget( format, parent ),
    m_vertexBuffer( QGLBuffer::VertexBuffer ),
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

// количество вершин куба
static const uint32_t cubeVerticesCount = 24;

// описание геометрии куба для всех его сторон
// координаты вершин куба
const float s = 1.0f; // половина размера куба
const float cubePositions[cubeVerticesCount][3] = {
        {-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
        { s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
        {-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
        { s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
        {-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
        { s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
};

// cube normals
const float cubeNormals[cubeVerticesCount][3] = {
        { 0, 0,-s}, { 0, 0,-s}, { 0, 0,-s}, { 0, 0,-s}, // front
        { 0, 0, s}, { 0, 0, s}, { 0, 0, s}, { 0, 0, s}, // back
        { 0, s, 0}, { 0, s, 0}, { 0, s, 0}, { 0, s, 0}, // top
        { 0,-s, 0}, { 0,-s, 0}, { 0,-s, 0}, { 0,-s, 0}, // bottom
        { s, 0, 0}, { s, 0, 0}, { s, 0, 0}, { s, 0, 0}, // left
        {-s, 0, 0}, {-s, 0, 0}, {-s, 0, 0}, {-s, 0, 0}  // right
};

// текстурные координаты куба
const float cubeTexcoords[cubeVerticesCount][2] = {
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
        {0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
};

// количество индексов куба
const uint32_t cubeIndicesCount = 36;

// индексы вершин куба в порядке поротив часовой стрелки
const uint32_t cubeIndices[cubeIndicesCount] = {
         0, 3, 1,  1, 3, 2, // front
         4, 7, 5,  5, 7, 6, // back
         8,11, 9,  9,11,10, // top
        12,15,13, 13,15,14, // bottom
        16,19,17, 17,19,18, // left
        20,23,21, 21,23,22  // right
};

GLuint texture;

void GLRenderWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Prepare a complete shader program...
    if ( !prepareShaderProgram( "shaders/vert.glsl", "shaders/frag.glsl" ) )
        return;

    m_timer = new QTimer( this );
    m_timer->start( 10 );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

    QImage texImage( "images/default.png" );
    texture = bindTexture( texImage );

    // vertex positions
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_vertexBuffer.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( cubePositions, cubeVerticesCount * 3 * sizeof( float ) );
    m_vertexBuffer.release();

    // vertex texcoords
    m_texcoordBuffer.create();
    m_texcoordBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_texcoordBuffer.bind() )
    {
        qWarning() << "Could not bind texcoord buffer to the context";
        return;
    }
    m_texcoordBuffer.allocate( cubeTexcoords, cubeVerticesCount * 2 * sizeof( float ) );
    m_texcoordBuffer.release();

    // indices
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_indexBuffer.bind() )
    {
        qWarning() << "Could not bind index buffer to the context";
        return;
    }
    m_indexBuffer.allocate( cubeIndices, cubeIndicesCount * sizeof( uint32_t ) );
    m_texcoordBuffer.release();

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    // Enable the "vertex" attribute to bind it to our currently bound
    // vertex buffer.
    m_vertexBuffer.bind();
    m_shader.setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
    m_vertexBuffer.release();

    m_texcoordBuffer.bind();
    m_shader.setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
    m_texcoordBuffer.release();


    m_shader.release();

    m_fpsTime->start();
    m_workTime->start();

    glEnable( GL_DEPTH_TEST );

    m_camera->setPosition( Eigen::Vector3f( 0.0, 0.0, -6.0 ) );
}

bool GLRenderWidget::prepareShaderProgram( const QString& vertexShaderPath,
                                           const QString& fragmentShaderPath )
{
    // First we load and compile the vertex shader...
    bool result = m_shader.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !result )
        qWarning() << m_shader.log();

    // ...now the fragment shader...
    result = m_shader.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !result )
        qWarning() << m_shader.log();

    // ...and finally we link them to resolve any references.
    result = m_shader.link();
    if ( !result )
        qWarning() << "Could not link shader program:" << m_shader.log();

    return result;
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

void GLRenderWidget::paintGL()
{
    applyInput();

    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QMatrix4x4 matrix( m_camera->viewProjectionMatrix() );

    m_shader.bind();

    m_shader.setUniformValue( "colorTexture", 0 );

    m_vertexBuffer.bind();
    m_texcoordBuffer.bind();
    m_indexBuffer.bind();
    m_shader.enableAttributeArray( "position" );
    m_shader.enableAttributeArray( "texcoord" );

    // Draw stuff

    glBindTexture( GL_TEXTURE_2D, texture );

    m_shader.setUniformValue( "modelViewProjectionMatrix", matrix );
    m_shader.setUniformValue( "baseColor", QVector4D( 0.5, 0.0, 0.0, 0.0 ) );
    glDrawElements( GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL );

    matrix.translate( 3.0, 0.0, 3.0 );
    m_shader.setUniformValue( "modelViewProjectionMatrix", matrix );
    m_shader.setUniformValue( "baseColor", QVector4D( 0.0, 0.0, 0.0, 0.0 ) );
    glDrawElements( GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL );

    glBindTexture( GL_TEXTURE_2D, 0 );

    m_shader.disableAttributeArray( "position" );
    m_shader.disableAttributeArray( "texcoord" );
    m_vertexBuffer.release();
    m_texcoordBuffer.release();
    m_indexBuffer.release();
    m_shader.release();



    m_text.add( "CAMERA\t", m_camera->position() );
    m_text.add( "FPS\t", m_fps );
    m_text.draw( this, 10, 15 );
    m_text.clear();

    nextFrame();
}

void GLRenderWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;
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
