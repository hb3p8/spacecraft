#include "GLRenderWidget.hpp"

#include <QGLShader>
#include <QKeyEvent>
#include <QApplication>
#include <QMatrix>
#include <QImage>


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

    QImage texImage( "images/grass_top.jpg" );
    texture = bindTexture( texImage );
/*
    float points[] = { -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f,
                            0.0f,  0.5f, 0.0f };*/

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

void GLRenderWidget::paintGL()
{
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    const float aspectRatio = (float)width() / (float)height();
    matrix.perspective( 45.0f, aspectRatio, 1.0f, 100.0f );
    matrix.translate( 0.0f, 0.0f, -4.0f );
    matrix.rotate( 30.0f, 0.0f, 1.0f, 0.0f );



    glBindTexture( GL_TEXTURE_2D, texture );
    m_shader.bind();

    m_shader.setUniformValue( "modelViewProjectionMatrix", matrix );
    m_shader.setUniformValue( "colorTexture", 0 );

    m_vertexBuffer.bind();
    m_texcoordBuffer.bind();
    m_indexBuffer.bind();
    m_shader.enableAttributeArray( "position" );
    m_shader.enableAttributeArray( "texcoord" );

    // Draw stuff
  //  glDrawArrays( GL_TRIANGLES, 0, 10 );
    glDrawElements( GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL );

    m_shader.disableAttributeArray( "position" );
    m_shader.disableAttributeArray( "texcoord" );
    m_vertexBuffer.release();
    m_texcoordBuffer.release();
    m_indexBuffer.release();
    m_shader.release();
    glBindTexture( GL_TEXTURE_2D, 0 );


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

        default:
            QGLWidget::keyPressEvent( e );
    }
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
