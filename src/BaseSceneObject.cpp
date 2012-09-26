#include "BaseSceneObject.hpp"

BaseSceneObject::BaseSceneObject():
  m_position( Eigen::Vector3f::Zero() ),
  m_velocity( Eigen::Vector3f::Zero() ),
  m_rotation( Eigen::AngleAxisf( 0.0, Eigen::Vector3f::UnitX() ) ),
  m_angularVelocity( Eigen::Vector3f::Zero() ),
  m_massCenter( Eigen::Vector3f::Zero() )
{}
