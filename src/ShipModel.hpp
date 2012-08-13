#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"


#define SHIP_MAX_SIZE 32

typedef unsigned int BlockData;

class ShipModel
{
public:
    ShipModel();

    inline BlockData& getBlock( int x, int y, int z ) { return m_blocks[ x ][ y ][ z ]; }
    bool octreeIntersect( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Intersection& intersection );

private:
    Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

    BlockData m_blocks[SHIP_MAX_SIZE][SHIP_MAX_SIZE][SHIP_MAX_SIZE];

    Eigen::Vector3i m_center;
    Octree m_octree;

};

#endif // SHIP_MODEL_H
