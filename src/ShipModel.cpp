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
        if( /*j < 4 && i < 4 && k < 4 */ ( j == 0 ) || ( j < 3 && rand() % 10 > 6 )  )
          m_blocks[ i ][ j ][ k ] = 1;
        else
          m_blocks[ i ][ j ][ k ] = 0;
      }

  m_octree.build( this );
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



