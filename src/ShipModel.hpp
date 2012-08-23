#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "MeshData/Cube.hpp"


#define SHIP_MAX_SIZE_DEFAULT 32

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
  typedef std::vector<BlockRef> BlockRefArray;

  ShipModel( size_t size = SHIP_MAX_SIZE_DEFAULT );
  ShipModel( std::string fileName );
  ~ShipModel();

  inline BlockData& getBlock( int x, int y, int z ) { return *( m_blocks + x + m_size * y + m_size * m_size * z ); }
  inline BlockData& getBlock( BlockRef block ){ return getBlock( block.i, block.j, block.k ); }
  bool octreeRaycastIntersect( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Intersection& intersection );

  void refreshModel();
  void saveToFile( std::string fileName );
  void loadFromFile( std::string fileName, bool reallocateBlocks = false );

  void optimize();

  Octree& getOctree() { return m_octree; }
  Mesh& getMesh() { return m_mesh; }
  size_t getSize() { return m_size; }

  Eigen::Vector3f calculateMassCenter();

  Eigen::Vector3f getMassCenter(){ return m_massCenter; }
  float getMass(){ return m_mass; }
  float getInertia( Eigen::Vector3f axis );

  void findEngines();
  BlockRefArray getEngines() { return m_engines; }


private:
  Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

  float sqrDistToAxis( Eigen::Vector3f& axis, Eigen::Vector3f& point );

  void buildMesh();

  BlockData* m_blocks;

  size_t m_size;

  Eigen::Vector3i m_center;
  Octree m_octree;
  Mesh m_mesh;

  Eigen::Vector3f m_massCenter;
  float m_mass;

  BlockRefArray m_engines;

};

#endif // SHIP_MODEL_H
