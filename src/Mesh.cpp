#include "Mesh.hpp"

Mesh::Mesh():
  m_vertexBuffer( QGLBuffer::VertexBuffer ),
  m_normalBuffer( QGLBuffer::VertexBuffer ),
  m_texcoordBuffer( QGLBuffer::VertexBuffer ),
  m_indexBuffer( QGLBuffer::IndexBuffer ),
  m_colorBuffer( QGLBuffer::VertexBuffer ),
  m_mode( GL_TRIANGLES )
{
}

void Mesh::setMode( int mode )
{
   this->m_mode = mode;
}

void Mesh::attachShader( QGLShaderProgram& shader )
{
  m_shaderProgram = &shader;

//  m_vertexBuffer.bind();
//  shader.setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
//  m_vertexBuffer.release();

//  m_normalBuffer.bind();
//  shader.setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
//  m_normalBuffer.release();

//  m_texcoordBuffer.bind();
//  shader.setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
//  m_texcoordBuffer.release();

//  m_colorBuffer.bind();
//  shader.setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
//  m_colorBuffer.release();
}

void Mesh::drawIndexed()
{

  if( !isAllocated( m_indexBuffer ) || !isAllocated( m_vertexBuffer ) )
  {
    qWarning() << "You must allocate buffers to draw mesh!";
    return;
  }

  m_vertexBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
  m_vertexBuffer.release();
  m_shaderProgram->enableAttributeArray( "position" );

  if( isAllocated( m_normalBuffer ) )
  {
    m_normalBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
    m_normalBuffer.release();

    m_shaderProgram->enableAttributeArray( "normal" );
  }

  if( isAllocated( m_texcoordBuffer ) )
  {
    m_texcoordBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
    m_texcoordBuffer.release();

    m_shaderProgram->enableAttributeArray( "texcoord" );
  }

  if( isAllocated( m_colorBuffer ) )
  {
    m_colorBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
    m_colorBuffer.release();

    m_shaderProgram->enableAttributeArray( "color" );
  }

  m_indexBuffer.bind();

  glDrawElements( m_mode, m_indicesCount, GL_UNSIGNED_INT, NULL );

  m_shaderProgram->disableAttributeArray( "position" );
  m_shaderProgram->disableAttributeArray( "normal" );
  m_shaderProgram->disableAttributeArray( "texcoord" );
  m_shaderProgram->disableAttributeArray( "color" );
}

void Mesh::drawSimple()
{
  static bool indexWarning = true;

  if( !isAllocated( m_vertexBuffer ) )
  {
    qWarning() << "You must allocate buffers to draw mesh!";
    return;
  }

  if( isAllocated( m_indexBuffer ) && indexWarning )
  {
    indexWarning = false;
    qWarning() << "Indices allocated, so you probably have to call drawIndexed() instead of drawSimple()";
  }

  m_vertexBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
  m_vertexBuffer.release();
  m_shaderProgram->enableAttributeArray( "position" );

  if( isAllocated( m_normalBuffer ) )
  {
    m_normalBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
    m_normalBuffer.release();

    m_shaderProgram->enableAttributeArray( "normal" );
  }

  if( isAllocated( m_texcoordBuffer ) )
  {
    m_texcoordBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
    m_texcoordBuffer.release();

    m_shaderProgram->enableAttributeArray( "texcoord" );
  }

  if( isAllocated( m_colorBuffer ) )
  {
    m_colorBuffer.bind();
    m_shaderProgram->setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
    m_colorBuffer.release();

    m_shaderProgram->enableAttributeArray( "color" );
  }

  glDrawArrays( m_mode, 0, m_verticesCount );

  m_shaderProgram->disableAttributeArray( "position" );
  m_shaderProgram->disableAttributeArray( "normal" );
  m_shaderProgram->disableAttributeArray( "texcoord" );
  m_shaderProgram->disableAttributeArray( "color" );
}

void Mesh::writeIndexedData( const void* positionsData, const void* normalsData,
                                    const void* texcoordsData, const void* indicesData, const void* colorsData,
                                    uint32_t verticesCount, uint32_t indicesCount )
{
  writePositions( positionsData, verticesCount );
  writeNormals( normalsData, verticesCount );
  writeTexcoords( texcoordsData, verticesCount );
  writeColors( colorsData, verticesCount );
  writeIndices( indicesData, indicesCount );
}


void Mesh::writeSimpleData( const void* positionsData, const void* texcoordsData, uint32_t verticesCount )
{
  writePositions( positionsData, verticesCount );
  writeTexcoords( texcoordsData, verticesCount );
}

bool Mesh::isAllocated( QGLBuffer& buffer )
{
  AllocationMap::const_iterator i;

  i = m_bufferAllocationMap.find( &buffer );
  return ( i != m_bufferAllocationMap.end() && i.value() == true );
}

void Mesh::writePositions( const void *positionsData, uint32_t count )
{
  // vertex positions
  m_vertexBuffer.create();
  m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_vertexBuffer.bind() )
  {
      qWarning() << "Could not bind vertex buffer to the context";
      return;
  }
  m_vertexBuffer.allocate( positionsData, count * 3 * sizeof( float ) );
  m_vertexBuffer.release();

  m_verticesCount = count;

  m_bufferAllocationMap.insert( &m_vertexBuffer, true );
}

void Mesh::writeNormals( const void *normalsData, uint32_t count )
{
  // vertex normals
  m_normalBuffer.create();
  m_normalBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_normalBuffer.bind() )
  {
      qWarning() << "Could not bind normal buffer to the context";
      return;
  }
  m_normalBuffer.allocate( normalsData, count * 3 * sizeof( float ) );
  m_normalBuffer.release();

  m_bufferAllocationMap.insert( &m_normalBuffer, true );
}

void Mesh::writeTexcoords( const void *texcoordsData, uint32_t count )
{
  // vertex texcoords
  m_texcoordBuffer.create();
  m_texcoordBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_texcoordBuffer.bind() )
  {
      qWarning() << "Could not bind texcoord buffer to the context";
      return;
  }
  m_texcoordBuffer.allocate( texcoordsData, count * 2 * sizeof( float ) );
  m_texcoordBuffer.release();

  m_bufferAllocationMap.insert( &m_texcoordBuffer, true );
}

void Mesh::writeColors( const void *colorsData, uint32_t count )
{
  // vertex colors
  m_colorBuffer.create();
  m_colorBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_colorBuffer.bind() )
  {
      qWarning() << "Could not bind color buffer to the context";
      return;
  }
  m_colorBuffer.allocate( colorsData, count * 3 * sizeof( float ) );
  m_colorBuffer.release();

  m_bufferAllocationMap.insert( &m_colorBuffer, true );
}

void Mesh::writeIndices( const void *indicesData, uint32_t count )
{
  // indices
  m_indexBuffer.create();
  m_indexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_indexBuffer.bind() )
  {
      qWarning() << "Could not bind index buffer to the context";
      return;
  }
  m_indexBuffer.allocate( indicesData, count * sizeof( uint32_t ) );
  m_indexBuffer.release();

  m_indicesCount = count;

  m_bufferAllocationMap.insert( &m_indexBuffer, true );
}


