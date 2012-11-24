#include "Camera.hpp"

#include <math.h>

#include <QVector3D>

using namespace Eigen;

Camera::Camera ( const Eigen::Vector3f &position, const Eigen::Vector3f &rotation):
  m_position( position ),
  m_curRotation( rotation ),
  m_viewportWidth( 1024 ),
  m_viewportHeight( 768 ),
  m_fieldOfView( 60.0 ),
  m_near( 0.1 ),
  m_far( 10000 )
{
    setRotation( m_curRotation );
}

Camera::Camera ( const Camera &camera ) :
  m_position( camera.m_position ),
  m_curRotation( camera.m_curRotation ),
  m_rotation( camera.m_rotation ),
  m_viewportWidth( camera.m_viewportWidth ),
  m_viewportHeight( camera.m_viewportHeight ),
  m_fieldOfView( camera.m_fieldOfView ),
  m_near( camera.m_near ),
  m_far( camera.m_far )
{
}

QMatrix4x4 Camera::projectionMatrix() const
{
  QMatrix4x4 matrix;
  matrix.setToIdentity();
  const float aspectRatio = m_viewportWidth / m_viewportHeight;
  matrix.perspective( m_fieldOfView, aspectRatio, m_near, m_far );

  return matrix;
}

QMatrix4x4 Camera::viewMatrix() const
{

  QMatrix4x4 matrix;
  matrix.setToIdentity();

  QVector3D eyeVector( position().x(),
                       position().y(),
                       position().z() );

  QVector3D centerVector( position().x() + view().x(),
                          position().y() + view().y(),
                          position().z() + view().z() );

  QVector3D upVector( up().x(),
                      up().y(),
                      up().z() );

  matrix.lookAt( eyeVector, centerVector, upVector );

  return matrix;
}

Vector3f Camera::position() const
{
  return m_position;
}

void Camera::setPosition( const Vector3f &position )
{
  m_position = position;
}

AngleAxisf Camera::rotation() const
{
  return m_rotation;
}

void Camera::setRotation( const Vector3f& rotation )
{
  m_rotation = AngleAxisf( rotation.z(), Vector3f::UnitZ() ) *
              AngleAxisf( rotation.y(), Vector3f::UnitY() ) *
              AngleAxisf( rotation.x(), Vector3f::UnitX() );
}

void Camera::setRotation( const AngleAxisf& rotation )
{
  m_rotation = rotation;
}

void Camera::eyeTurn( float dx, float dy )
{
  m_curRotation.x() += dy;

  // don't allow head to flip over
  m_curRotation.x() = m_curRotation.x() < M_PI / 2 ? m_curRotation.x() : M_PI / 2;
  m_curRotation.x() = m_curRotation.x() > -M_PI / 2 ? m_curRotation.x() : -M_PI / 2;

  m_curRotation.y() -= dx;
  if (m_curRotation.y() < -M_PI)
    m_curRotation.y() += M_PI * 2;
  if (m_curRotation.y() > M_PI)
    m_curRotation.y() -= M_PI * 2;

  setRotation( m_curRotation );
}

void Camera::rotate( const Vector3f& delta )
{
  m_rotation = m_rotation *
    AngleAxisf( delta.z(), Vector3f::UnitZ() ) *
    AngleAxisf( delta.y(), Vector3f::UnitY() ) *
    AngleAxisf( delta.x(), Vector3f::UnitX() );
}

void Camera::translate( const Vector3f& delta )
{
  m_position += delta;
}

Vector3f Camera::view() const
{
  return ( m_rotation * Vector3f( 0, 0, 1 ) ).normalized();
}

Vector3f Camera::up() const
{
  return ( m_rotation * Vector3f( 0, 1, 0 ) ).normalized();
}

Vector3f Camera::right() const
{
  return ( m_rotation * Vector3f( 1, 0, 0 ) ).normalized();
}
