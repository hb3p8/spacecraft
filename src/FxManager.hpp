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
//    void add( Eigen::Matrix4f transform, void* userdata );
    virtual void draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix ) = 0;

  };

  class Lines : public EffectType
  {
  public:
    void initialize( QGLWidget* widget );

    void draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix );

    void addLine( Eigen::Vector3f p1, Eigen::Vector3f p2 );

  private:
    Mesh m_mesh;
    QGLShaderProgram m_shader;
    GLuint m_texture;

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
