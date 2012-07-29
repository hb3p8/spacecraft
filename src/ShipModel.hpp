#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>


#define SHIP_MAX_SIZE 32

typedef unsigned int BlockData;

class ShipModel
{
public:
    ShipModel();

    inline BlockData& getBlock( int x, int y, int z ) { return m_blocks[ x ][ y ][ z ]; }

private:    
    BlockData m_blocks[SHIP_MAX_SIZE][SHIP_MAX_SIZE][SHIP_MAX_SIZE];

    Eigen::Vector3i m_center;

};

#endif // SHIP_MODEL_H
