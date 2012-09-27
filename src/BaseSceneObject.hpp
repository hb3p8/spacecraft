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

  Eigen::Vector3d getMassCenter(){ return m_massCenter; }

  Eigen::Vector3d& position() { return m_position; }
  Eigen::AngleAxisd& rotation() { return m_rotation; }

protected:

  Eigen::Vector3d m_position;
  Eigen::Vector3d m_velocity;

  Eigen::AngleAxisd m_rotation;
  Eigen::Vector3d m_angularVelocity;

  Eigen::Vector3d m_massCenter;

};

typedef std::shared_ptr< BaseSceneObject > BaseSceneObjectPtr;

#endif // BASE_SCENE_OBJECT_HPP
