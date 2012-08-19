#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Core>
#include <QString>
#include <QGLShaderProgram>
#include <QVector3D>

#define SIDE_NO_INTERSECTION 6

struct Intersection
{
  size_t i, j, k;
  float time;
  size_t side;

  inline bool intersected() { return side != SIDE_NO_INTERSECTION; }
};

inline QVector3D eigenVectorToQt( Eigen::Vector3f v ) { return QVector3D( v.x(), v.y(), v.z() ); }
inline Eigen::Vector3f qtVectorToEigen( QVector3D v ) { return Eigen::Vector3f( v.x(), v.y(), v.z() ); }

bool rayBoxIntersection( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Eigen::Vector3f boxMin,
                         Eigen::Vector3f boxMax, float* time, size_t* side );

int directionSideTest( Eigen::Vector3f rayDir );

bool prepareShaderProgram( QGLShaderProgram& program, const QString& vertexShaderPath,
                           const QString& fragmentShaderPath );

#endif // UTILS_HPP
