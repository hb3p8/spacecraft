#include "SimpleMesh.hpp"

SimpleMesh::SimpleMesh():
  m_vertexBuffer( QGLBuffer::VertexBuffer ),
  m_normalBuffer( QGLBuffer::VertexBuffer ),
  m_texcoordBuffer( QGLBuffer::VertexBuffer ),
  m_allocated( false )
{

}

void SimpleMesh::allocateBuffers( const void* positionsData, const void* normalsData,
                            const void* texcoordsData, size_t verticesCount )
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

  m_allocated = true;
  m_verticesCount = verticesCount;
}


void SimpleMesh::setAttributesToShader( QGLShaderProgram& shader )
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
}

void SimpleMesh::draw()
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

  m_shaderProgram->enableAttributeArray( "position" );
  m_shaderProgram->enableAttributeArray( "normal" );
  m_shaderProgram->enableAttributeArray( "texcoord" );

  glDrawArrays( GL_TRIANGLES, 0, m_verticesCount );

  m_shaderProgram->disableAttributeArray( "position" );
  m_shaderProgram->disableAttributeArray( "normal" );
  m_shaderProgram->disableAttributeArray( "texcoord" );
}

