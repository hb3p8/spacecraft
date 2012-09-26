#ifndef BASE_SCENE_OBJECT_HPP
#define BASE_SCENE_OBJECT_HPP

#include <Eigen/Eigen>
#include <memory>
#include <QGLShaderProgram>

class BaseSceneObject
{
public:
  BaseSceneObject();

  virtual ~BaseSceneObject(){}

  virtual void process( float deltaTime ) {}
  virtual void draw() = 0;
  virtual void attachShader( QGLShaderProgram& shader ) {}
  virtual void refreshModel() {}

  Eigen::Vector3f getMassCenter(){ return m_massCenter; }

  Eigen::Vector3f& position() { return m_position; }
  Eigen::AngleAxisf& rotation() { return m_rotation; }

protected:

  Eigen::Vector3f m_position;
  Eigen::Vector3f m_velocity;

  Eigen::AngleAxisf m_rotation;
  Eigen::Vector3f m_angularVelocity;

  Eigen::Vector3f m_massCenter;

};

typedef std::shared_ptr< BaseSceneObject > BaseSceneObjectPtr;

#endif // BASE_SCENE_OBJECT_HPP
