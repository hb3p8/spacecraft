#include "Mesh.hpp"

Mesh::Mesh():
  m_vertexBuffer( QGLBuffer::VertexBuffer ),
  m_normalBuffer( QGLBuffer::VertexBuffer ),
  m_texcoordBuffer( QGLBuffer::VertexBuffer ),
  m_indexBuffer( QGLBuffer::IndexBuffer ),
  m_colorBuffer( QGLBuffer::VertexBuffer ),
  m_allocated( false )
{

}

Mesh* Mesh::CreateCube()
{
  Mesh* cube = new Mesh();
  cube->allocateBuffers( cubePositions, cubeNormals, cubeTexcoords,
                         cubeIndices, cubeColors, cubeVerticesCount, cubeIndicesCount );

  return cube;
}

void Mesh::allocateBuffers( const void* positionsData, const void* normalsData,
                            const void* texcoordsData, const void* indicesData, const void* colorsData,
                            size_t verticesCount, size_t indicesCount )
{
  // vertex positions
  m_vertexBuffer.create();
  m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_vertexBuffer.bind() )
  {
      qWarning() << "Could not bind vertex buffer to the context";
      return;
  }
  m_vertexBuffer.allocate( positionsData, verticesCount * 3 * sizeof( float ) );
  m_vertexBuffer.release();

  // vertex normals
  m_normalBuffer.create();
  m_normalBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_normalBuffer.bind() )
  {
      qWarning() << "Could not bind normal buffer to the context";
      return;
  }
  m_normalBuffer.allocate( normalsData, verticesCount * 3 * sizeof( float ) );
  m_normalBuffer.release();

  // vertex texcoords
  m_texcoordBuffer.create();
  m_texcoordBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_texcoordBuffer.bind() )
  {
      qWarning() << "Could not bind texcoord buffer to the context";
      return;
  }
  m_texcoordBuffer.allocate( texcoordsData, verticesCount * 2 * sizeof( float ) );
  m_texcoordBuffer.release();

  // indices
  m_indexBuffer.create();
  m_indexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_indexBuffer.bind() )
  {
      qWarning() << "Could not bind index buffer to the context";
      return;
  }
  m_indexBuffer.allocate( indicesData, indicesCount * sizeof( uint32_t ) );
  m_indexBuffer.release();

  // vertex colors
  m_colorBuffer.create();
  m_colorBuffer.setUsagePattern( QGLBuffer::StaticDraw );
  if ( !m_colorBuffer.bind() )
  {
      qWarning() << "Could not bind color buffer to the context";
      return;
  }
  m_colorBuffer.allocate( colorsData, verticesCount * 3 * sizeof( float ) );
  m_colorBuffer.release();

  m_indicesCount = indicesCount;
  m_allocated = true;
}

void Mesh::writeData( const void *positionsData, const void *normalsData, const void *texcoordsData,
                      const void *indicesData, const void *colorsData, uint32_t verticesCount, uint32_t indicesCount )
{
  // TODO: write почему-то не работает
//  if( !m_allocated )
  {
    allocateBuffers( positionsData, normalsData, texcoordsData, indicesData,
                     colorsData, verticesCount, indicesCount );

    return;
  }

//  m_vertexBuffer.bind();
//  m_vertexBuffer.write( 0, positionsData, verticesCount * 3 * sizeof( float ) );

//  m_normalBuffer.bind();
//  m_normalBuffer.write( 0, normalsData, verticesCount * 3 * sizeof( float ) );

//  m_colorBuffer.bind();
//  m_colorBuffer.write( 0, colorsData, verticesCount * 3 * sizeof( float ) );

//  m_texcoordBuffer.bind();
//  m_texcoordBuffer.write( 0, texcoordsData, verticesCount * 2 * sizeof( float ) );

//  m_indexBuffer.bind();
//  m_indexBuffer.write( 0, indicesData, indicesCount * sizeof( uint32_t ) );

//  m_indexBuffer.release();

}

void Mesh::setAttributesToShader( QGLShaderProgram& shader )
{
  m_shaderProgram = &shader;

  if( !m_allocated ) return;

  m_vertexBuffer.bind();
  shader.setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
  m_vertexBuffer.release();

  m_normalBuffer.bind();
  shader.setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
  m_normalBuffer.release();

  m_texcoordBuffer.bind();
  shader.setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
  m_texcoordBuffer.release();

  m_colorBuffer.bind();
  shader.setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
  m_colorBuffer.release();
}

void Mesh::draw()
{
  if( !m_allocated ) return;

  m_vertexBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
  m_vertexBuffer.release();

  m_normalBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "normal", GL_FLOAT, 0, 3 );
  m_normalBuffer.release();

  m_texcoordBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "texcoord", GL_FLOAT, 0, 2 );
  m_texcoordBuffer.release();

  m_colorBuffer.bind();
  m_shaderProgram->setAttributeBuffer( "color", GL_FLOAT, 0, 3 );
  m_colorBuffer.release();

  m_indexBuffer.bind();

  m_shaderProgram->enableAttributeArray( "position" );
  m_shaderProgram->enableAttributeArray( "normal" );
  m_shaderProgram->enableAttributeArray( "texcoord" );
  m_shaderProgram->enableAttributeArray( "color" );

  glDrawElements( GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, NULL );

  m_shaderProgram->disableAttributeArray( "position" );
  m_shaderProgram->disableAttributeArray( "normal" );
  m_shaderProgram->disableAttributeArray( "texcoord" );
  m_shaderProgram->disableAttributeArray( "color" );
}

