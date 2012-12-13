#ifndef FXMANAGER_HPP
#define FXMANAGER_HPP

#include <Eigen/Core>
#include <map>
#include <memory>
#include <string>

#include "Mesh.hpp"
#include <QGLShaderProgram>
#include <QMap>

class QGLWidget;

namespace spacefx
{

  class EffectType
  {
  public:
    virtual void draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix ) = 0;

  };

  class Lines : public EffectType
  {
  public:
    Lines(): m_vertexBuffer( QGLBuffer::VertexBuffer ),
      m_verticesCount( 0 ),
//      m_maxVerticesCount( 512 ),
      m_data( 64 )
    {}

    void initialize( QGLWidget* widget );

    void draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix );

    void addLine( Eigen::Vector3f p1, Eigen::Vector3f p2 );
    void addLine( Eigen::Vector3d p1, Eigen::Vector3d p2 );

  private:

    QGLShaderProgram m_shader;
    GLuint m_texture;
    QGLBuffer m_vertexBuffer;
    size_t m_verticesCount;
//    size_t m_maxVerticesCount;
    std::vector<Eigen::Vector3f> m_data;

  };

  class Sprites : public EffectType
  {

  };

  typedef std::shared_ptr< EffectType > EffectTypePtr;

  typedef QMap< QString, EffectTypePtr > EffectMap;


  class Manager
  {

  public:
    Manager();

    void registerEffect( const QString& id, EffectTypePtr effect );
    EffectTypePtr getEffect( const QString& id );

    void drawEffects( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix );

  private:
    EffectMap m_effects;

  };


}

#endif
