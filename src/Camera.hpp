#ifndef BASE_CAMERA_HPP
#define BASE_CAMERA_HPP

#include <Eigen/Geometry>

//#include <boost/tr1/memory.hpp>
#include <memory>

#include <QMatrix4x4>


/*! Basic camera. */
class Camera
{
public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  Camera ( const Eigen::Vector3f &position = Eigen::Vector3f::Zero(),
           const Eigen::Vector3f &rotation = Eigen::Vector3f::Zero() );

  Camera ( const Camera &camera );

  void viewportResize( int width, int height ) { m_width = width; m_height = height; }
  void setFieldOfView( float FOV ) { m_fieldOfView = FOV; }

  QMatrix4x4 projectionMatrix() const;
  QMatrix4x4 viewMatrix() const;

  Eigen::Vector3f position() const;

  void setPosition( const Eigen::Vector3f &position );

  void setRotation( const Eigen::Vector3f& rotation );
  void setRotation( const Eigen::AngleAxisf& rotation );

  Eigen::AngleAxisf rotation() const;

  void eyeTurn( float dx, float dy );

  void rotate( const Eigen::Vector3f &delta );

  void translate( const Eigen::Vector3f &delta );


  Eigen::Vector3f view() const;
  Eigen::Vector3f up() const;
  Eigen::Vector3f right() const;

private:

  Eigen::Vector3f m_position;

  Eigen::Vector3f m_curRotation;
  Eigen::AngleAxisf m_rotation;

  float m_width;
  float m_height;
  float m_fieldOfView;
  float m_near;
  float m_far;
};

typedef std::shared_ptr< Camera > CameraPtr;


#endif
