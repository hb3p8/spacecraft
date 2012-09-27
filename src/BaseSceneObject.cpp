#include "BaseSceneObject.hpp"

BaseSceneObject::BaseSceneObject():
  m_position( Eigen::Vector3d::Zero() ),
  m_velocity( Eigen::Vector3d::Zero() ),
  m_rotation( Eigen::AngleAxisd( 0.0, Eigen::Vector3d::UnitX() ) ),
  m_angularVelocity( Eigen::Vector3d::Zero() ),
  m_massCenter( Eigen::Vector3d::Zero() )
{}
