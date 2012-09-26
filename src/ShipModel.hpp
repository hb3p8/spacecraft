#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "MeshData/Cube.hpp"
#include "BaseSceneObject.hpp"


#define SHIP_MAX_SIZE_DEFAULT 32

struct BlockData
{
  unsigned int blockType;
  unsigned char orientation; // интерпретируется как side

  BlockData(): blockType( 0 ), orientation( 0 ) {}

  inline bool isEmpty() { return blockType == 0; }
};

class ShipModel: public BaseSceneObject
{
public:
  typedef std::vector< BlockRef > BlockRefArray;
  typedef QMap< int, double > InertiaCash;
  typedef QMap< int, float > EngineFloatMap;

  ShipModel( size_t size = SHIP_MAX_SIZE_DEFAULT );
  ShipModel( std::string fileName );
  ~ShipModel();

  inline BlockData& getBlock( int x, int y, int z ) { return *( m_blocks + x + m_size * y + m_size * m_size * z ); }
  inline BlockData& getBlock( BlockRef block ){ return getBlock( block.i, block.j, block.k ); }
  bool octreeRaycastIntersect( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, Intersection& intersection );

  void refreshModel();
  void saveToFile( std::string fileName );
  void loadFromFile( std::string fileName, bool reallocateBlocks = false );

  void process( float deltaTime );

  void optimize();

  void draw() { m_mesh.drawIndexed(); }
  void attachShader( QGLShaderProgram& shader ){ m_mesh.attachShader( shader ); }

  Octree& getOctree() { return m_octree; }
  Mesh& getMesh() { return m_mesh; }
  size_t getSize() { return m_size; }

  Eigen::Vector3d calculateMassCenter();

  float getMass(){ return m_mass; }
  float getInertia( Eigen::Vector3d axis, BlockRef block, int side );

  void findEngines();
  BlockRefArray& getEngines() { return m_engines; }
  EngineFloatMap& enginePower() { return m_enginePower; }

  size_t modelSize() { return m_size; }


private:
  void initialize();

  Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

  double sqrDistToAxis( Eigen::Vector3d& axis, Eigen::Vector3d& point );

  void buildMesh();

  BlockData* m_blocks;

  size_t m_size;

  Eigen::Vector3i m_center;
  Octree m_octree;
  Mesh m_mesh;

  float m_mass;

  BlockRefArray m_engines;
  InertiaCash m_inertiaCash;

  EngineFloatMap m_enginePower;


};

#endif // SHIP_MODEL_H
