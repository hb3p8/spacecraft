#include "Camera.hpp"

#include <QVector3D>

using namespace Eigen;

Camera::Camera ( const Eigen::Vector3f &position,
                 const Eigen::Quaternionf &orientation ) :
                                  m_position ( position ),
                                  m_orientation ( orientation )
{
}

//------------------------------------------------------------------------

Camera::Camera ( const Camera &camera ) :
                                  m_position ( camera.m_position ),
                                  m_orientation ( camera.m_orientation )
//                                  m_offsetX ( camera._offsetX ),
//                                  m_offsetY ( camera._offsetY ),
//                                  m_width ( camera._width ),
//                                  m_height ( camera._height ),
//                                  m_nearPlane ( camera._nearPlane ),
//                                  m_farPlane ( camera._farPlane ),
//                                  m_fieldOfView ( camera._fieldOfView )
{
}

QMatrix4x4 Camera::projectionMatrix() const
{
  QMatrix4x4 matrix;
  matrix.setToIdentity();
  const float aspectRatio = 800. / 600.;
  matrix.perspective( 45.0f, aspectRatio, 1.0f, 100.0f );

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

//------------------------------------------------------------------------

Vector3f Camera::position() const
{
  return m_position;
}

//------------------------------------------------------------------------

void Camera::setPosition( const Vector3f &position )
{
  m_position = position;
}

//------------------------------------------------------------------------

Quaternionf Camera::orientation( void ) const
{
  return m_orientation;
}

//------------------------------------------------------------------------

void Camera::setOrientation( const Quaternionf &orientation )
{
  m_orientation = orientation;
}

//------------------------------------------------------------------------


void Camera::setOrientation( const AngleAxisf &orientation )
{
  m_orientation = Quaternionf( orientation );
}

//------------------------------------------------------------------------

void Camera::rotate( const Quaternionf &delta )
{
  setOrientation( orientation() * delta );
}

//------------------------------------------------------------------------

void Camera::rotate( const Eigen::AngleAxisf &delta )
{
  setOrientation( orientation() * Quaternionf( delta ) );
}

//------------------------------------------------------------------------

void Camera::translate( const Vector3f &delta )
{
  setPosition( position() + orientation() * delta );
}

//------------------------------------------------------------------------

Vector3f Camera::view( void ) const
{
  return orientation() * Vector3f::UnitZ();
}

//------------------------------------------------------------------------

Vector3f Camera::up( void ) const
{
  return orientation() * Vector3f::UnitY();
}

//------------------------------------------------------------------------

Vector3f Camera::right( void ) const
{
  return orientation() * Vector3f::UnitX();
}
