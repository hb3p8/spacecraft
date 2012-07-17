#include "Mesh.hpp"

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

Mesh::Mesh():
  m_vertexBuffer( QGLBuffer::VertexBuffer ),
  m_normalBuffer( QGLBuffer::VertexBuffer ),
  m_texcoordBuffer( QGLBuffer::VertexBuffer ),
  m_indexBuffer( QGLBuffer::IndexBuffer )
{
  allocateBuffers( cubePositions, cubeNormals, cubeTexcoords,
                         cubeIndices, cubeVerticesCount, cubeIndicesCount );
}

Mesh* Mesh::CreateCube()
{
  Mesh* cube = new Mesh();

  return cube;
}

void Mesh::allocateBuffers( const void* positionsData, const void* normalsData,
                            const void* texcoordsData, const void* indicesData,
                            size_t verticesCount, size_t IndicesCount )
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
  m_indexBuffer.allocate( indicesData, IndicesCount * sizeof( uint32_t ) );
  m_texcoordBuffer.release();
}

void Mesh::setAttributesToShader( QGLShaderProgram& shader )
{
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

void Mesh::draw()
{
  m_indexBuffer.bind();
  glDrawElements( GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL );
}
