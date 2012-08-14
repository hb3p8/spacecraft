#ifndef MESH_H
#define MESH_H

#include <QGLBuffer>
#include <QGLShader>

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

const float cubeColors[cubeVerticesCount][3] = {
        { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, // front
        { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, // back
        { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, // top
        { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, // bottom
        { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, // left
        { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }  // right
};

// количество индексов куба
const uint32_t cubeIndicesCount = 36;

// индексы вершин куба в порядке поротив часовой стрелки
const uint32_t cubeIndices[cubeIndicesCount] = {
         0, 3, 1,  1, 3, 2, // front 5
         4, 7, 5,  5, 7, 6, // back 2
         8,11, 9,  9,11,10, // top 4
        12,15,13, 13,15,14, // bottom 1
        16,19,17, 17,19,18, // left 0
        20,23,21, 21,23,22  // right 3
};

class Mesh
{
public:
    Mesh();

    static Mesh* CreateCube();

    void writeData( const void* positionsData, const void* normalsData,
                    const void* texcoordsData, const void* indicesData, const void* colorsData,
                    uint32_t verticesCount, uint32_t IndicesCount );

    void setAttributesToShader( QGLShaderProgram& shader );
    void draw();

private:

    void allocateBuffers( const void* positionsData, const void* normalsData,
                          const void* texcoordsData, const void* indicesData, const void* colorsData,
                          uint32_t verticesCount, uint32_t IndicesCount );

    QGLShaderProgram* m_shaderProgram;
    QGLBuffer m_vertexBuffer;
    QGLBuffer m_normalBuffer;
    QGLBuffer m_texcoordBuffer;
    QGLBuffer m_indexBuffer;
    QGLBuffer m_colorBuffer;

    bool m_allocated;
    size_t m_indicesCount;
};

#endif // MESH_H
