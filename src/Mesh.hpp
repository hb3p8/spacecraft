#ifndef MESH_H
#define MESH_H

#include <QGLBuffer>
#include <QGLShader>
#include <QMap>

class Mesh
{
public:
    Mesh();

    void writeIndexedData( const void* positionsData, const void* normalsData,
                           const void* texcoordsData, const void* indicesData, const void* colorsData,
                           uint32_t verticesCount, uint32_t IndicesCount );
    void writeSimpleData( const void* positionsData, const void* texcoordsData, uint32_t verticesCount );

    void writePositions( const void* positionsData, uint32_t count );
    void writeNormals( const void* normalsData, uint32_t count );
    void writeTexcoords( const void* texcoordsData, uint32_t count );
    void writeColors( const void* colorsData, uint32_t count );
    void writeIndices( const void* indicesData, uint32_t count );

    void attachShader( QGLShaderProgram& shader );

    // draw mesh using index buffer
    void drawIndexed();

    // draw mesh without index buffer
    void drawSimple();

    void setMode( int mode );


private:
    typedef QMap< QGLBuffer*, bool >  AllocationMap;

    bool isAllocated( QGLBuffer& buffer );

    QGLShaderProgram* m_shaderProgram;
    QGLBuffer m_vertexBuffer;
    QGLBuffer m_normalBuffer;
    QGLBuffer m_texcoordBuffer;
    QGLBuffer m_indexBuffer;
    QGLBuffer m_colorBuffer;

    AllocationMap m_bufferAllocationMap;
    size_t m_indicesCount;
    size_t m_verticesCount;

    int m_mode;
};

#endif // MESH_H
