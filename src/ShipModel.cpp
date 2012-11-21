#include "ShipModel.hpp"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <QFile>

using namespace Eigen;
using namespace std;

const int numBlockTypes = 4;
int blockSpecs[ numBlockTypes ][ 6 ] =
{
  { 0, 0, 0, 0, 0, 0 }, // hull
  { 1, 1, 1, 1, 1, 1 }, // armor
  { 2, 2, 2, 2, 2, 2 }, // power
  { 3, 0, 0, 0, 0, 0 }  // engine
};


ShipModel::ShipModel( size_t size ): BaseSceneObject(), m_size( size )
{
  int center = m_size / 2;
  m_center = Vector3i( center, center, center );

  m_blocks = new BlockData[ m_size * m_size * m_size ];

  for( size_t i = 0; i < m_size; i++ )
    for( size_t j = 0; j < m_size; j++ )
      for( size_t k = 0; k < m_size; k++ )
      {
        if( /*j < 4 && i < 4 && k < 4 */ ( j == 0 ) /*|| ( j < 3 && rand() % 10 > 6 ) */ )
          getBlock( i, j, k ).blockType = 1;
        else
          getBlock( i, j, k ).blockType = 0;
      }
}

ShipModel::ShipModel( std::string fileName, bool noGraphics ):
  BaseSceneObject(),
  m_blocks( NULL ),
  m_noGraphics( noGraphics )
{
  loadFromFile( fileName, true );
}

ShipModel::~ShipModel()
{
  delete[] m_blocks;
}

void ShipModel::buildMesh()
{
  // преобразование индекса в side (формат side см в utils)
  int axisToSideRemap[ 7 ] = { 2, 1, 0, 6, 3, 4, 5 };

  int rowToSideRemap[ 6 ] = { 5, 2, 4, 1, 0, 3 };

  float* vertices;
  float* normals;
  float* texcoords;
  float* colors;
  uint32_t* indices;

  size_t verticesSize = m_octree.getRoot()->blocks().size() * cubeVerticesCount * 3;
  size_t texcoordsSize = m_octree.getRoot()->blocks().size() * cubeVerticesCount * 2;
  size_t indicesSize = m_octree.getRoot()->blocks().size() * cubeIndicesCount;

  vertices = new float[ verticesSize ];
  normals = new float[ verticesSize ];
  texcoords = new float[ texcoordsSize ];
  colors = new float[ verticesSize ];
  indices = new uint32_t[ indicesSize ];

  long int vertCounter = 0;
  long int idxCounter = 0;

  for( BlockRef& block : m_octree.getRoot()->blocks() )
  {
    size_t i = block.i;
    size_t j = block.j;
    size_t k = block.k;

    // аккумулятор окклюдеров на каждую сторону куба + ещё один
    // элемент массива для удобства
    int occluders[ 7 ] = { 0, 0, 0, 0, 0, 0, -1 };
    for( int x = -1; x <= 1; x++ )
      for( int y = -1; y <= 1; y++ )
        for( int z = -1; z <= 1; z++ )
        {
          int offset[3] = { x, y, z };

          if( ( i + x < 0 ) || ( j + y < 0 ) || ( k + z < 0 ) ||
              ( i + x >= m_size ) || ( j + y >= m_size ) || ( k + z >= m_size ) )
            continue;

          for( int axis = 1; axis <= 3; axis++ )
          {
            int index = axis * offset[ axis - 1 ] + 3;

            // если сторона ещё не помечена и по offset'у расположен блок
            if( occluders[ axisToSideRemap[ index ] ] >= 0 )
            {
              if( !getBlock( i + offset[ 0 ],
                             j + offset[ 1 ],
                             k + offset[ 2 ] ).isEmpty() )
              {
                // если окклюдер прямо напротив блока, помечаем сторону на удаление
                if( ( offset[ ( axis ) % 3 ] == 0 ) &&  ( offset[ ( axis + 1 ) % 3 ] == 0 ) )
                  occluders[ axisToSideRemap[ index ] ] = -1;
                else // иначе, прибавляем его к сумме для стороны
                  occluders[ axisToSideRemap[ index ] ] += 1;
              }
            }
          }
        }

    // генерим меш

    memcpy( normals + vertCounter * 3, cubeNormals, cubeVerticesCount * 3 * sizeof( float ) );
//    memcpy( texcoords + vertCounter * 2, cubeTexcoords, cubeVerticesCount * 2 * sizeof( float ) );

    Vector3f translation( 2.0 * i, 2.0 * j, 2.0 * k );

    BlockData& block = getBlock( i, j, k );

    float itemSize = 0.2;

    for( size_t x = 0; x < cubeIndicesCount; x++ )
    {
      int row = x / 6;
      int side = rowToSideRemap[ row ];

      // если сторона помечена на удаление из результирующего меша - не генерим для неё индексы
      if( occluders[ side ] != -1 )
      {
        int index = cubeIndices[ x ] + vertCounter;

        indices[ idxCounter ] = index;

        float* color = colors + index * 3;
        float ao = 0.6f + 0.4f * ( 9 - occluders[ side ] ) / 9.;
        color[ 0 ] = color[ 1 ] = color[ 2 ] = ao;

        side = rotateSide( side, block.orientation );

        int blockId = block.blockType - 1;
        int subTexId = blockSpecs[ blockId ][ side ];


        int idx = index % 4;
        float* tex = texcoords + index * 2;
        switch( idx )
        {
        case 0:
          tex[ 0 ] = subTexId * itemSize;
          tex[ 1 ] = 1;
          break;
        case 1:
          tex[ 0 ] = itemSize + subTexId * itemSize;
          tex[ 1 ] = 1;
          break;
        case 2:
          tex[ 0 ] = itemSize + subTexId * itemSize;
          tex[ 1 ] = 0.8;
          break;
        case 3:
          tex[ 0 ] = subTexId * itemSize;
          tex[ 1 ] = 0.8;
          break;
        }

        idxCounter++;
      }
    }

    for( size_t i = 0; i < cubeVerticesCount; i++ )
    {
      float* vert = vertices + vertCounter * 3;
      vert[ 0 ] = cubePositions[ i ][ 0 ] + translation.x();
      vert[ 1 ] = cubePositions[ i ][ 1 ] + translation.y();
      vert[ 2 ] = cubePositions[ i ][ 2 ] + translation.z();

      vertCounter++;
    }


  }

  m_mesh.writeIndexedData( vertices, normals, texcoords, indices, colors,
                           m_octree.getRoot()->blocks().size() * cubeVerticesCount,
                           idxCounter );

  delete[] vertices;
  delete[] normals;
  delete[] texcoords;
  delete[] colors;
  delete[] indices;

}

void ShipModel::refreshModel()
{
  m_octree.build( this );

  if( !m_noGraphics )
    buildMesh();

  calculateMassCenter();
  findEngines();
  m_inertiaCash.clear();
}

bool ShipModel::octreeRaycastIntersect( Vector3f rayStart, Vector3f rayDir, Intersection& intersection )
{
  assert( m_octree.getRoot() != 0 );
  intersection = traverse( rayStart, rayDir, *m_octree.getRoot() );

  return intersection.side != SIDE_NO_INTERSECTION;
}

// TODO: может перенести это куда-нибудь ближе к октри
Intersection ShipModel::traverse( Vector3f rayStart, Vector3f rayDir, OctreeNode& node )
{
  float newTime;
  size_t newSide;
  Intersection minIntersection = { 0, 0, 0, 1e+10, SIDE_NO_INTERSECTION };

  Vector3f min( 2. * node.minBorder.x() - 1., 2. * node.minBorder.y() - 1., 2. * node.minBorder.z() -1. );
  Vector3f max( 2. * node.maxBorder.x() + 1., 2. * node.maxBorder.y() + 1., 2. * node.maxBorder.z() +1. );

  if( rayBoxIntersection( rayStart, rayDir, min, max, &newTime, &newSide ) )
  {
    if( node.isLeaf() )
    {
      for( BlockRef& block : node.blocks() )
      {
        Vector3f cubeMin( 2. * block.i - 1., 2. * block.j - 1., 2. * block.k - 1.);
        Vector3f cubeMax( 2. * block.i + 1., 2. * block.j + 1., 2. * block.k + 1.);

        if( rayBoxIntersection( rayStart, rayDir, cubeMin, cubeMax, &newTime, &newSide ) )
        {
          if( newTime < minIntersection.time )
          {
            minIntersection.i = block.i;
            minIntersection.j = block.j;
            minIntersection.k = block.k;
            minIntersection.time = newTime;
            minIntersection.side = newSide;
          }
        }
      }
    } else
    {
      for( OctreeNodePtr& childNode : node.children() )
      {
        Intersection childIntersect = traverse( rayStart, rayDir, *childNode );
        if( childIntersect.time < minIntersection.time )
          minIntersection = childIntersect;
      }
    }
  }

  return minIntersection;
}

void ShipModel::saveToFile( string fileName )
{
  ofstream outfile;
  outfile.open( fileName );

  vector< BlockRef >& blocks = m_octree.getRoot()->blocks();

  outfile << m_size << endl;
  outfile << blocks.size() << endl;


  for( BlockRef& block : blocks )
  {
    BlockData& blockData = getBlock( block.i, block.j, block.k );
    assert( blockData.blockType < 5 ); //!
    outfile << block.i << "\t" << block.j << "\t" << block.k << "\t" <<
               blockData.blockType << "\t" << blockData.orientation << endl;
  }

  outfile.close();
}

void ShipModel::loadFromFile( string fileName, bool reallocateBlocks )
{
  assert( QFile::exists( fileName.c_str() ) );

  ifstream infile;
  infile.open( fileName );

  size_t worldDim;
  infile >> worldDim;

  if( ( reallocateBlocks && ( m_size != worldDim )  )|| ( m_blocks == NULL ) )
  {
    if( m_blocks != NULL )
    {
      delete[] m_blocks;
    }

    m_size = worldDim;
    m_blocks = new BlockData[ m_size * m_size * m_size ];
  }

  if( worldDim != m_size )
  {
    qWarning() << "Size doesn't match. Loading canceled.";
    infile.close();
    return;
  }

  memset( m_blocks, 0, m_size * m_size * m_size * sizeof( BlockData ) );

  size_t size;
  infile >> size;

  for( size_t c = 0; c < size; c++ )
  {
    int i, j, k, blockType;
    char blockOrient;

    infile >> i;
    infile >> j;
    infile >> k;
    infile >> blockType;
    infile >> blockOrient;

    getBlock( i, j, k ).blockType = blockType;
    assert( blockType < 5 ); //!
    getBlock( i, j, k ).orientation = blockOrient;
  }

  infile.close();
}

void ShipModel::optimize()
{
  size_t minBorder[ 3 ] = { m_size, m_size, m_size };
  size_t maxBorder[ 3 ] = { 0, 0, 0 };

  for( size_t i = 0; i < m_size; i++ )
    for( size_t j = 0; j < m_size; j++ )
      for( size_t k = 0; k < m_size; k++ )
      {
        if( ( j == 0 ) || ( getBlock( i, j, k ).isEmpty() ) ) continue;

        minBorder[ 0 ] = qMin( minBorder[ 0 ], i );
        minBorder[ 1 ] = qMin( minBorder[ 1 ], j );
        minBorder[ 2 ] = qMin( minBorder[ 2 ], k );

        maxBorder[ 0 ] = qMax( maxBorder[ 0 ], i );
        maxBorder[ 1 ] = qMax( maxBorder[ 1 ], j );
        maxBorder[ 2 ] = qMax( maxBorder[ 2 ], k );
      }

  size_t newSize = qMax( maxBorder[ 2 ] - minBorder[ 2 ] + 1,
                   qMax( maxBorder[ 0 ] - minBorder[ 0 ] + 1, maxBorder[ 1 ] - minBorder[ 1 ] + 1 ) );
  newSize += newSize % 2;

  BlockData* newBlocks = new BlockData[ newSize * newSize * newSize ];
  auto setNewBlock = [&]( int i, int j, int k, BlockData& block )
    { newBlocks[ i + newSize * j + newSize * newSize * k ] = block; };

  memset( newBlocks, 0, newSize * newSize * newSize * sizeof( BlockData ) );

  for( size_t i = 0; i < newSize; i++ )
    for( size_t j = 0; j < newSize; j++ )
      for( size_t k = 0; k < newSize; k++ )
      {

        if( ( i + minBorder[ 0 ] >= m_size ) ||
            ( j + minBorder[ 1 ] >= m_size ) ||
            ( k + minBorder[ 2 ] >= m_size ) ) continue;

        setNewBlock( i, j, k, getBlock( i + minBorder[ 0 ], j + minBorder[ 1 ], k + minBorder[ 2 ] ) );
      }

  delete[] m_blocks;

  m_blocks = newBlocks;
  m_size = newSize;

  m_octree.build( this );

}

Eigen::Vector3d ShipModel::calculateMassCenter()
{
  double massSum = 0.0;
  Vector3d massCenter = Vector3d::Zero();

  vector< BlockRef >& blocks = m_octree.getRoot()->blocks();

  for( BlockRef& block : blocks )
  {
    double blockMass = 1.0;
    massSum += blockMass;

    massCenter += blockMass * Vector3d( 2. * block.i, 2. * block.j, 2. * block.k );
  }

  m_massCenter = massCenter / massSum;
  m_mass = massSum;

  return m_massCenter;
}

double ShipModel::sqrDistToAxis( Vector3d& axis, Vector3d& pos )
{
  Vector3d distToMass = m_massCenter - pos;

  return ( distToMass - axis * ( distToMass.dot( axis ) / axis.squaredNorm() ) ).squaredNorm();
}

float ShipModel::getInertia( Vector3d axis, BlockRef engineBlock, int side )
{
  InertiaCash::const_iterator i;

  i = m_inertiaCash.find( engineBlock.generalIndex( m_size, getBlock( engineBlock ).orientation ) );
  if( i != m_inertiaCash.end() )
    return i.value();

  double inertia = 0;

  vector< BlockRef >& blocks = m_octree.getRoot()->blocks();

  for( BlockRef& block : blocks )
  {
    Vector3d blockPos( 2. * block.i, 2. * block.j, 2. * block.k );
    float mass = 1.0;

    inertia += sqrDistToAxis( axis, blockPos ) * mass;
  }

  m_inertiaCash.insert( engineBlock.generalIndex( m_size, side ), inertia );
  return inertia;
}

void ShipModel::findEngines()
{
  m_engines.clear();

  vector< BlockRef >& blocks = m_octree.getRoot()->blocks();

  for( BlockRef& block : blocks )
  {
    if( getBlock( block ).blockType == 4 )
      m_engines.push_back( block );
  }
}

void ShipModel::process( float deltaTime )
{

  for( BlockRef engineBlockRef : getEngines() )
  {
    EngineFloatMap::const_iterator i;
    int idx = engineBlockRef.generalIndex( m_size, getBlock( engineBlockRef ).orientation );
    i = m_enginePower.find( idx );
    if( i == m_enginePower.end() || i.value() < 1e-6 )
      continue;

    float power = i.value();

    int side = 0;
    side = rotateSide( side, getBlock( engineBlockRef ).orientation );

    Vector3d engineDir = (-1) * sideToNormal( side );

    float engineForce = 0.1 * power;

    Vector3d enginePos( engineBlockRef.position_double( 2. ) );
    Vector3d angularVelDelta = engineDir.cross( enginePos - m_massCenter );

    // изменение угловой скорости считаем в локальном пространстве модели
    m_angularVelocity += (-1) * angularVelDelta * engineForce /
        getInertia( angularVelDelta, engineBlockRef, side );

    // изменение скорости - в мировом пространстве (с учётом shipRotation)
    engineDir = m_rotation * engineDir;
    m_velocity += engineDir * engineForce / m_mass;

  }

  m_position += m_velocity * deltaTime;
  assert( m_velocity.x() != 1/0. && m_velocity.y() != 1/0. &&  m_velocity.z() != 1/0. );

  if( m_angularVelocity.norm() > 1e-5 )
    m_rotation = m_rotation *
        AngleAxisd( m_angularVelocity.norm() * deltaTime, m_angularVelocity.normalized() );


}




