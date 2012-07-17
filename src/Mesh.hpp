#ifndef MESH_H
#define MESH_H

#include <QGLBuffer>
#include <QGLShader>

class Mesh
{
public:
    Mesh();

    static Mesh* CreateCube();
    void allocateBuffers( const void* positionsData, const void* normalsData,
                          const void* texcoordsData, const void* indicesData,
                          uint32_t verticesCount, uint32_t IndicesCount );

    void setAttributesToShader( QGLShaderProgram& shader );
    void draw();

private:

    QGLBuffer m_vertexBuffer;
    QGLBuffer m_normalBuffer;
    QGLBuffer m_texcoordBuffer;
    QGLBuffer m_indexBuffer;
};

#endif // MESH_H
