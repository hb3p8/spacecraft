#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "IndexedMesh.hpp"


#define SHIP_MAX_SIZE 32

struct BlockData
{
  unsigned int blockType;
  unsigned char orientation; // интерпретируется как side

  BlockData(): blockType( 0 ), orientation( 0 ) {}

  inline bool isEmpty() { return blockType == 0; }
};

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
    IndexedMesh& getMesh() { return m_mesh; }



private:
    Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

    void buildMesh();

    BlockData m_blocks[SHIP_MAX_SIZE][SHIP_MAX_SIZE][SHIP_MAX_SIZE];

    Eigen::Vector3i m_center;
    Octree m_octree;
    IndexedMesh m_mesh;

};

#endif // SHIP_MODEL_H
