#include "ShipModel.hpp"
#include <assert.h>

using namespace Eigen;


ShipModel::ShipModel()
{
  int center = SHIP_MAX_SIZE / 2;
  m_center = Vector3i( center, center, center );

  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
        if( /*j < 4 && i < 4 && k < 4 */ ( j == 0 ) /*|| ( j < 3 && rand() % 10 > 6 )*/  )
          m_blocks[ i ][ j ][ k ] = 1;
        else
          m_blocks[ i ][ j ][ k ] = 0;
      }

  refreshModel();
}

void ShipModel::recalculateAO()
{
  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
        if( m_blocks[ i ][ j ][ k ] == 0 ) continue;

        int occluders = 0;
        for( int x = -1; x <= 1; x++ )
          for( int y = -1; y <= 1; y++ )
            for( int z = -1; z <= 1; z++ )
            {
              if( ( i + x < 0 ) || ( j + y < 0 ) || ( k + z < 0 ) ||
                  ( i + x >= SHIP_MAX_SIZE ) || ( j + y >= SHIP_MAX_SIZE ) || ( k + z >= SHIP_MAX_SIZE ) )
                continue;

              if( m_blocks[ i + x ][ j + y ][ k + z ] > 0 )
                occluders++;
            }
        m_blocks[ i ][ j ][ k ] = 27 - occluders + 1;
      }
}

void ShipModel::refreshModel()
{
  m_octree.build( this );

  recalculateAO();
}

bool ShipModel::octreeIntersect( Vector3f rayStart, Vector3f rayDir, Intersection& intersection )
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


//int clamp( int val, int lo, int hi )
//{
//  int result = val < lo ? lo : val;
//  result = val > hi ? hi : val;
//  return result;
//}

//Vector3i clamp( Vector3i vec, int lo, int hi )
//{
//  return Vector3i( clamp( vec.x(), lo, hi ),
//                   clamp( vec.y(), lo, hi ),
//                   clamp( vec.z(), lo, hi ) );
//}



