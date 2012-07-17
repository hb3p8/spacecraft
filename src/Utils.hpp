#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Core>
#include <QString>
#include <QGLShaderProgram>

#define SIDE_NO_INTERSECTION 7

bool rayBoxIntersection( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Eigen::Vector3f boxMin,
                         Eigen::Vector3f boxMax, float* time, size_t* side );

bool prepareShaderProgram( QGLShaderProgram& program, const QString& vertexShaderPath,
                           const QString& fragmentShaderPath );

#endif // UTILS_HPP
