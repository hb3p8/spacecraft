#ifndef MESH_H
#define MESH_H

#include <QGLBuffer>
#include <QGLShader>


class SimpleMesh
{
public:
    SimpleMesh();

    void allocateBuffers( const void* positionsData, const void* normalsData,
                          const void* texcoordsData, uint32_t verticesCount );


    void setAttributesToShader( QGLShaderProgram& shader );
    void draw();

private:

    QGLShaderProgram* m_shaderProgram;
    QGLBuffer m_vertexBuffer;
    QGLBuffer m_normalBuffer;
    QGLBuffer m_texcoordBuffer;

    size_t m_verticesCount;

    bool m_allocated;
};

#endif // MESH_H
