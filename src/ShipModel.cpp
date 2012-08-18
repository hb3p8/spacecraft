#include "ShipModel.hpp"
#include <assert.h>
#include <iostream>
#include <fstream>

using namespace Eigen;
using namespace std;

const int numBlockTypes = 4;
int blockSpecs[ numBlockTypes ][ 6 ] =
{
  { 0, 0, 0, 0, 0, 0 }, // hull
  { 1, 1, 1, 1, 1, 1 }, // armor
  { 2, 2, 2, 2, 2, 2 }, // power
  { 2, 0, 0, 0, 0, 0 }  // engine
};

ShipModel::ShipModel()
{
  int center = SHIP_MAX_SIZE / 2;
  m_center = Vector3i( center, center, center );

  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
        if( /*j < 4 && i < 4 && k < 4 */ ( j == 0 ) /*|| ( j < 3 && rand() % 10 > 6 ) */ )
          m_blocks[ i ][ j ][ k ].blockType = 1;
        else
          m_blocks[ i ][ j ][ k ].blockType = 0;
      }

//  refreshModel();
}

void ShipModel::buildMesh()
{
  // преобразование индекса в side (формат side см в utils)
  int axisToSideRemap[ 7 ] = { 2, 1, 0, 6, 3, 4, 5 };

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
              ( i + x >= SHIP_MAX_SIZE ) || ( j + y >= SHIP_MAX_SIZE ) || ( k + z >= SHIP_MAX_SIZE ) )
            continue;

          for( int axis = 1; axis <= 3; axis++ )
          {
            int index = axis * offset[ axis - 1 ] + 3;

            // если сторона ещё не помечена и по offset'у расположен блок
            if( occluders[ axisToSideRemap[ index ] ] >= 0 )
            {
              if( !m_blocks[ i + offset[ 0 ] ]
                           [ j + offset[ 1 ] ]
                           [ k + offset[ 2 ] ].isEmpty() )
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

    int rowToSideRemap[ 6 ] = { 5, 2, 4, 1, 0, 3 };

    memcpy( normals + vertCounter * 3, cubeNormals, cubeVerticesCount * 3 * sizeof( float ) );

    Vector3f translation( 2.0 * i, 2.0 * j, 2.0 * k );

    float itemSize = 0.2;

    for( size_t x = 0; x < cubeIndicesCount; x++ )
    {
      int row = x / 6;
      int side = rowToSideRemap[ row ];

      // если сторона помечена на удаление из результирующего меша - не генерим для неё индексы
      if( occluders[ side ] != -1 )
      {
        indices[ idxCounter ] = cubeIndices[ x ] + vertCounter;

        float* color = colors + indices[ idxCounter ] * 3;
        float ao = 0.6f + 0.4f * ( 9 - occluders[ side ] ) / 9.;
        color[ 0 ] = color[ 1 ] = color[ 2 ] = ao;

        int blockId = m_blocks[ i ][ j ][ k ].blockType - 1;
        int subTexId = blockSpecs[ blockId ][ side ];

        int idx = indices[ idxCounter ] % 4;
        float* tex = texcoords + indices[ idxCounter ] * 2;
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

  m_mesh.writeData( vertices, normals, texcoords, indices, colors,
                    m_octree.getRoot()->blocks().size() * cubeVerticesCount,
                    indicesSize );

  delete[] vertices;
  delete[] normals;
  delete[] texcoords;
  delete[] colors;
  delete[] indices;

}

void ShipModel::refreshModel()
{
  m_octree.build( this );

  buildMesh();
}

bool ShipModel::octreeRaycastIntersect( Vector3f rayStart, Vector3f rayDir, Intersection& intersection )
{
  assert( m_octree.getRoot() != 0 );
  intersection = traverse( rayStart, rayDir, *m_octree.getRoot() );

  return intersection.side != SIDE_NO_INTERSECTION;
}

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

  outfile << SHIP_MAX_SIZE << endl;
  outfile << blocks.size() << endl;

  for( BlockRef& block : blocks )
  {
    outfile << block.i << "\t" << block.j << "\t" << block.k << "\t" <<
               m_blocks[ block.i ][ block.j ][ block.k ].blockType << endl;
  }

  outfile.close();
}

void ShipModel::loadFromFile( string fileName )
{
  // clean
  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
          m_blocks[ i ][ j ][ k ].blockType = 0;
      }

  ifstream infile;
  infile.open( fileName );

  int worldDim;
  infile >> worldDim;
  assert( worldDim == SHIP_MAX_SIZE );

  size_t size;
  infile >> size;

  for( size_t c = 0; c < size; c++ )
  {
    int i, j, k, block;

    infile >> i;
    infile >> j;
    infile >> k;
    infile >> block;

    m_blocks[ i ][ j ][ k ].blockType = block;
  }

  infile.close();
}



