#include "ShipModel.hpp"

using namespace Eigen;


ShipModel::ShipModel()
{
  int center = SHIP_MAX_SIZE / 2;
  m_center = Vector3i( center, center, center );

  for( size_t i = 0; i < SHIP_MAX_SIZE; i++ )
    for( size_t j = 0; j < SHIP_MAX_SIZE; j++ )
      for( size_t k = 0; k < SHIP_MAX_SIZE; k++ )
      {
        if( j == 0 || ( j < 3 && rand() % 10 > 6 )  )
          m_blocks[ i ][ j ][ k ] = 1;
        else
          m_blocks[ i ][ j ][ k ] = 0;
      }
}

int clamp( int val, int lo, int hi )
{
  int result = val < lo ? lo : val;
  result = val > hi ? hi : val;
  return result;
}

Vector3i clamp( Vector3i vec, int lo, int hi )
{
  return Vector3i( clamp( vec.x(), lo, hi ),
                   clamp( vec.y(), lo, hi ),
                   clamp( vec.z(), lo, hi ) );
}



