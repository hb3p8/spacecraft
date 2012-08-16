#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"


#define SHIP_MAX_SIZE 32

typedef unsigned int BlockData;

class ShipModel
{
public:
    ShipModel();

    inline BlockData& getBlock( int x, int y, int z ) { return m_blocks[ x ][ y ][ z ]; }
    bool octreeRaycastIntersect( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Intersection& intersection );

    void refreshModel();
    void saveToFile( std::string fileName );
    void loadFromFile( std::string fileName );

    Octree& getOctree() { return m_octree; }
    Mesh& getMesh() { return m_mesh; }



private:
    Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

    void buildMesh();

    BlockData m_blocks[SHIP_MAX_SIZE][SHIP_MAX_SIZE][SHIP_MAX_SIZE];

    Eigen::Vector3i m_center;
    Octree m_octree;
    Mesh m_mesh;

};

#endif // SHIP_MODEL_H
