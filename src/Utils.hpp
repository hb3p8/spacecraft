#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Core>
#include <QString>
#include <QGLShaderProgram>
#include <QVector3D>

#include "MeshData/Cube.hpp"

#define SIDE_NO_INTERSECTION 6

struct Intersection
{
  size_t i, j, k;
  float time;
  size_t side;

  inline bool intersected() { return side != SIDE_NO_INTERSECTION; }
};

struct BlockRef
{
  int i, j, k;
  BlockRef(): i( 0 ), j( 0 ), k( 0 ) {}
  BlockRef( int x, int y, int z ): i( x ), j( y ), k( z ) {}

  inline int generalIndex( int size, int side ) { return ( i + j * size + k * size * size ); }
  inline Eigen::Vector3f position( float blockSize )
    { return Eigen::Vector3f( blockSize * i, blockSize * j, blockSize * k ); }

  inline Eigen::Vector3d position_double( double blockSize )
    { return Eigen::Vector3d( blockSize * i, blockSize * j, blockSize * k ); }
};

inline QVector3D eigenVectorToQt( Eigen::Vector3f v ) { return QVector3D( v.x(), v.y(), v.z() ); }
inline QVector3D eigenVectorToQt( Eigen::Vector3d v ) { return QVector3D( v.x(), v.y(), v.z() ); }

inline Eigen::Vector3f qtVectorToEigen( QVector3D v ) { return Eigen::Vector3f( v.x(), v.y(), v.z() ); }

bool rayBoxIntersection( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Eigen::Vector3f boxMin,
                         Eigen::Vector3f boxMax, float* time, size_t* side );

int directionSideTest( Eigen::Vector3f rayDir );
int directionXZSideTest( Eigen::Vector3f rayDir );

int rotateSide( int side, int orient );

Eigen::Vector3d sideToNormal( int side );


bool prepareShaderProgram( QGLShaderProgram& program, const QString& vertexShaderPath,
                           const QString& fragmentShaderPath );

#endif // UTILS_HPP
