#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "IndexedMesh.hpp"


#define SHIP_MAX_SIZE_DEFAULT 40

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
    ShipModel( size_t size = SHIP_MAX_SIZE_DEFAULT );
    ~ShipModel();

    inline BlockData& getBlock( int x, int y, int z ) { return *( m_blocks + x + m_size * y + m_size * m_size * z ); }
    bool octreeRaycastIntersect( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Intersection& intersection );

    void refreshModel();
    void saveToFile( std::string fileName );
    void loadFromFile( std::string fileName );

    void optimize();

    Octree& getOctree() { return m_octree; }
    IndexedMesh& getMesh() { return m_mesh; }
    size_t getSize() { return m_size; }



private:
    Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

    void buildMesh();

    BlockData* m_blocks;

    size_t m_size;

    Eigen::Vector3i m_center;
    Octree m_octree;
    IndexedMesh m_mesh;

};

#endif // SHIP_MODEL_H
