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

  Camera ( const Eigen::Vector3f &position = Eigen::Vector3f::Zero ( ),
           const Eigen::Quaternionf &orientation =
                                             Eigen::Quaternionf::Identity ( ) );

  Camera ( const Camera &camera );

  QMatrix4x4 projectionMatrix() const;
  QMatrix4x4 viewMatrix() const;

//  /*! Returns camera transformation in general form. */
//  virtual Eigen::Transform3f transform() const = 0;
//  /*! Sets camera transformation in general form. */
//  virtual void setTransform( const Eigen::Transform3f &transform ) = 0;

  /*! Returns camera position. */
  Eigen::Vector3f position() const;
  /*! Sets camera position. */
  void setPosition( const Eigen::Vector3f &position );

  //! Returns orientation of the camera in the world space.
  Eigen::Quaternionf orientation ( void ) const;

  //! Sets orientation of the camera in the world space.
  void setOrientation ( const Eigen::Quaternionf &orientation );

  //! Sets orientation of the camera in the world space.
  void setOrientation ( const Eigen::AngleAxisf &orientation );

  /*! Rotates the camera in world space (using Euler angles). */
  void rotate( const Eigen::Quaternionf &delta );
  void rotate( const Eigen::AngleAxisf &delta );
  /*! Translates the camera in local space (x-right, y-up, z-view). */
  void translate( const Eigen::Vector3f &delta );

  /*! Returns view vector of the camera. */
  Eigen::Vector3f view() const;
  /*! Returns up vector of the camera. */
  Eigen::Vector3f up() const;
  /*! Returns right vector of the camera. */
  Eigen::Vector3f right() const;

protected:

  /*! Camera position. */
  Eigen::Vector3f m_position;

  Eigen::Quaternionf m_orientation;
};

typedef std::shared_ptr< Camera > CameraPtr;
//typedef std::tr1::shared_ptr< Camera > BaseCameraPtr;


#endif
