#include "Utils.hpp"

using namespace Eigen;


//////////////////////
// Side value spec
//////////////////////
/*               y
       ___4___   |   ___4___
      |       |  |  |       |
     3|  2/5  |0 | 2|  0/3  |5
      |_______|  |  |_______|
          1      |      1
 x <-------------|-------------> z
       ___2___   |
      |       |  |
     3|  1/4  |0 |
      |_______|  |
          5      |
                 z
*/


bool rayBoxIntersection( Vector3f rayStart, Vector3f rayDir, Vector3f boxMin,
                         Vector3f boxMax, float* time, size_t* side )
{
  Vector3f Max;
  Vector3f Min;
  const float eps = 1e-6;

  for( size_t i = 0; i < 3; i++ )
  {
    float localMin = ( boxMin[ i ] - rayStart[ i ] ) / ( rayDir[ i ] + eps );
    float localMax = ( boxMax[ i ] - rayStart[ i ] ) / ( rayDir[ i ] + eps );

    Max[ i ] = fmax( localMax, localMin );
    Min[ i ] = fmin( localMax, localMin );
  }

  Max.x() = fmin( Max.x(), fmin( Max.y(), Max.z() ) );

  size_t& k = *side;
  k = 0;
  for( size_t i = 1; i < 3; i++ )
  {
    if( Min[ i ] > Min.x() )
    {
      Min.x() = Min[ i ];
      k = i;
    }
  }

  float center = ( boxMin[ k ] + boxMax[ k ] ) / 2.;

  Vector3f point = rayStart + rayDir * Min.x();

  if( point[ k ] > center )
    k += 3;

  return Max.x() > ( *time = fmax( Min.x(), 0.0 ) );

}

int directionXZSideTest( Vector3f rayDir )
{
  int side;
  rayDir.y() = 0;

  int axis = 0;
  if( qAbs( rayDir.z() ) > qAbs( rayDir.x() ) )
    axis = 2;

  side = axis;
  if( rayDir[ axis ] <= 0 ) side += 3;

  return side;
}

int directionSideTest( Vector3f rayDir )
{
  int side;
  int axis = 0;

  float max = 0;
  for( size_t i = 0; i < 3; i++ )
  {
    if( qAbs( rayDir[ i ] ) > max )
    {
      max = qAbs( rayDir[ i ] );
      axis = i;
    }
  }

  side = axis;
  if( rayDir[ axis ] <= 0 ) side += 3;

  return side;
}

int blockSpecs[][ 6 ] =
{
  { 0, 0, 0, 0, 0, 0 }, // hull
  { 1, 1, 1, 1, 1, 1 }, // armor
  { 2, 2, 2, 2, 2, 2 }, // power
  { 3, 0, 0, 0, 0, 0 }, // engine
  { 4, 0, 0, 0, 0, 0 }  // gun
};

int getBlockSpecs( int blockId, int side )
{
  return blockSpecs[ blockId ][ side ];
}

void setBlockTexcoords( int index, int subTexId, int size_x, float itemSize, float* texcoords )
{
  int idx = index % 4;
  int subTexIdX = subTexId % size_x;
  int subTexIdY = subTexId / size_x;
  float* tex = texcoords + index * 2;
  switch( idx )
  {
  case 0:
    tex[ 0 ] = subTexIdX * itemSize;
    tex[ 1 ] = 1 - subTexIdY * itemSize;
    break;
  case 1:
    tex[ 0 ] = itemSize + subTexIdX * itemSize;
    tex[ 1 ] = 1 - subTexIdY * itemSize;
    break;
  case 2:
    tex[ 0 ] = itemSize + subTexIdX * itemSize;
    tex[ 1 ] = 1 - ( subTexIdY + 1 ) * itemSize;
    break;
  case 3:
    tex[ 0 ] = subTexIdX * itemSize;
    tex[ 1 ] = 1 - ( subTexIdY + 1 ) * itemSize;
    break;
  }
}

/*
 * int rotateSide( int side, int orient )
 *
 * С помощью orient закодировано вращение блока.
 * side это сторона из которой поворотами нужно получить нужную.
 * sideToOrientYRemap - отношение orient к количеству необходимых поворотов по Y
 * rotationYRemap - преобразование поворота
 */

int rotationYRemap[ 6 ] = { 2, 1, 3, 5, 4, 0 };
int rotationZRemap[ 6 ] = { 4, 0, 2, 1, 3, 5 };

int sideToOrientYRemap[ 6 ] = { 0, 0, 3, 2, 0, 1 };
int sideToOrientZRemap[ 6 ] = { 0, 1, 0, 0, 3, 0 };

int rotateSide( int side, int orient )
{
  assert( orient < 6 );

  int rotationsY = sideToOrientYRemap[ orient ];
  for( int c = 0; c < rotationsY; c++ )
    side = rotationYRemap[ side ];

  int rotationsZ = sideToOrientZRemap[ orient ];
  for( int c = 0; c < rotationsZ; c++ )
    side = rotationZRemap[ side ];

  return side;
}

Vector3d sideToNormald( int side )
{
  int sideToRowRemap[ 6 ] = { 4, 2, 0, 5, 3, 1 };

  int index = cubeIndices[ sideToRowRemap[ side ] * 6 ];
  return Vector3d( cubeNormals[ index ][ 0 ],
                   cubeNormals[ index ][ 1 ],
                   cubeNormals[ index ][ 2 ] );
}

Vector3f sideToNormalf( int side )
{
  int sideToRowRemap[ 6 ] = { 4, 2, 0, 5, 3, 1 };

  int index = cubeIndices[ sideToRowRemap[ side ] * 6 ];
  return Vector3f( cubeNormals[ index ][ 0 ],
                   cubeNormals[ index ][ 1 ],
                   cubeNormals[ index ][ 2 ] );
}


bool prepareShaderProgram( QGLShaderProgram& program,
                           const QString& vertexShaderPath,
                           const QString& fragmentShaderPath )
{
    // First we load and compile the vertex shader...
    bool result = program.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !result )
        qWarning() << program.log();

    // ...now the fragment shader...
    result = program.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !result )
        qWarning() << program.log();

    // ...and finally we link them to resolve any references.
    result = program.link();
    if ( !result )
        qWarning() << "Could not link shader program:" << program.log();

    return result;
}

bool prepareShaderProgram( QGLShaderProgram& program,
                           const QString& vertexShaderPath,
                           const QString& fragmentShaderPath,
                           const QString& geometryShaderPath )
{
    bool result = program.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !result )
        qWarning() << program.log();

    result = program.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !result )
        qWarning() << program.log();

    result = program.addShaderFromSourceFile( QGLShader::Geometry, geometryShaderPath );
    if ( !result )
        qWarning() << program.log();

    result = program.link();
    if ( !result )
        qWarning() << "Could not link shader program:" << program.log();

    return result;
}

