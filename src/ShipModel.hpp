#ifndef SHIP_MODEL_H
#define SHIP_MODEL_H

#include <Eigen/Eigen>
#include "Octree.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "MeshData/Cube.hpp"
<<<<<<< HEAD
=======
#include "BaseSceneObject.hpp"
>>>>>>> octree


#define SHIP_MAX_SIZE_DEFAULT 32

struct BlockData
{
  unsigned int blockType;
  unsigned char orientation; // интерпретируется как side

  BlockData(): blockType( 0 ), orientation( 0 ) {}

  inline bool isEmpty() { return blockType == 0; }
};

<<<<<<< HEAD
class ShipModel
=======
class ShipModel: public BaseSceneObject
>>>>>>> octree
{
public:
  typedef std::vector< BlockRef > BlockRefArray;
  typedef QMap< int, float > InertiaCash;
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

<<<<<<< HEAD
=======
  void draw() { m_mesh.drawIndexed(); }
  void attachShader( QGLShaderProgram& shader ){ m_mesh.attachShader( shader ); }

>>>>>>> octree
  Octree& getOctree() { return m_octree; }
  Mesh& getMesh() { return m_mesh; }
  size_t getSize() { return m_size; }

<<<<<<< HEAD
  Eigen::Vector3d calculateMassCenter();

  Eigen::Vector3d getMassCenter(){ return m_massCenter; }
  float getMass(){ return m_mass; }
  float getInertia( Eigen::Vector3d axis, BlockRef block );
=======
  Eigen::Vector3f calculateMassCenter();

  float getMass(){ return m_mass; }
  float getInertia( Eigen::Vector3f axis, BlockRef block, int side );
>>>>>>> octree

  void findEngines();
  BlockRefArray& getEngines() { return m_engines; }
  EngineFloatMap& enginePower() { return m_enginePower; }

<<<<<<< HEAD
  Eigen::Vector3d& shipPosition() { return m_shipPosition; }
  Eigen::AngleAxisd& shipRotation() { return m_shipRotation; }

=======
>>>>>>> octree
  size_t modelSize() { return m_size; }


private:
  void initialize();

  Intersection traverse( Eigen::Vector3f rayStart, Eigen::Vector3f rayDir, OctreeNode& node );

<<<<<<< HEAD
  float sqrDistToAxis( Eigen::Vector3d& axis, Eigen::Vector3d& point );
=======
  float sqrDistToAxis( Eigen::Vector3f& axis, Eigen::Vector3f& point );
>>>>>>> octree

  void buildMesh();

  BlockData* m_blocks;

  size_t m_size;

  Eigen::Vector3i m_center;
  Octree m_octree;
  Mesh m_mesh;

<<<<<<< HEAD
  Eigen::Vector3d m_massCenter;
=======
>>>>>>> octree
  float m_mass;

  BlockRefArray m_engines;
  InertiaCash m_inertiaCash;

<<<<<<< HEAD
  Eigen::Vector3d m_shipPosition;
  Eigen::Vector3d m_shipVelocity;

  Eigen::AngleAxisd m_shipRotation;
  Eigen::Vector3d m_shipAngularVelocity;

=======
>>>>>>> octree
  EngineFloatMap m_enginePower;


};

#endif // SHIP_MODEL_H
