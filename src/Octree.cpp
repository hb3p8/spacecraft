#include "Octree.hpp"

using namespace Eigen;
using namespace std;



OctreeNode::OctreeNode(): m_isLeaf( false ), m_parent( 0 )
{

}

Octree::Octree(): m_root( 0 )
{

}

Octree::~Octree()
{
  if( m_root != 0 ) delete m_root;

}

void Octree::cleanup()
{
  if( m_root != 0 ) delete m_root;
  m_root = 0;

}

void Octree::build(  ShipModel& model )
{
  cleanup();

  m_root = new OctreeNode();

  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
        if( model.getBlock( i, j, k ) > 0 )
          m_root->blocks().push_back( BlockRef( i, j, k ) );
      }

  m_root->maxBorder = Vector3i( SHIP_MAX_SIZE - 1, SHIP_MAX_SIZE - 1, SHIP_MAX_SIZE - 1 );
  m_root->minBorder = Vector3i( 0, 0, 0 );


}

void Octree::buildNode( OctreeNode* parent, int level )
{
  if( parent->blocks().size() < 1 )
  {
    //leaf
  } else
  {
    //node

    int halfX = parent->maxBorder.x() - parent->minBorder.x() / 2;
    int halfY = parent->maxBorder.y() - parent->minBorder.y() / 2;
    int halfZ = parent->maxBorder.z() - parent->minBorder.z() / 2;

    for( size_t x = 0; x < 2; x++ )
      for( size_t y = 0; y < 2; y++ )
        for( size_t z = 0; z < 2; z++ )
        {
          OctreeNode* node = new OctreeNode();
          node->minBorder = parent->minBorder + Vector3i( x * halfX, y * halfY, z * halfZ );
          node->maxBorder = parent->minBorder +
              Vector3i( ( x + 1 ) * halfX, ( y + 1 ) * halfY, ( z + 1 ) * halfZ );

          // TODO: вынести из цикла
          for( size_t u = 0; u < parent->blocks().size(); u++ )
          {
            int& i = parent->blocks()[ u ].i;
            int& j = parent->blocks()[ u ].j;
            int& k = parent->blocks()[ u ].k;

            if( i < node->maxBorder.x() && i >= node->minBorder.x() &&
                j < node->maxBorder.y() && j >= node->minBorder.y() &&
                k < node->maxBorder.z() && k >= node->minBorder.z() )
              node->blocks().push_back( parent->blocks()[ u ] );
          }

          parent->children().push_back( OctreeNodePtr( node ) );

          buildNode( node, level + 1 );
        }

  }


}

