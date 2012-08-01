#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <memory>
#include <Eigen/Eigen>

class ShipModel;

struct BlockRef
{
  int i, j, k;
  BlockRef(): i( 0 ), j( 0 ), k( 0 ) {}
  BlockRef( int x, int y, int z ): i( x ), j( y ), k( z ) {}
};

class OctreeNode;
typedef std::shared_ptr< OctreeNode > OctreeNodePtr;

class OctreeNode
{
public:

  OctreeNode();

  inline OctreeNode* parent() { return m_parent; }
  inline std::vector<OctreeNodePtr>& children() { return m_children; }
  inline std::vector<BlockRef>& blocks() { return m_blocks; }
  inline void makeLeaf() { m_isLeaf = true; }
  inline bool isLeaf() const { return m_isLeaf; }

  Eigen::Vector3i maxBorder;
  Eigen::Vector3i minBorder;

private:

  bool m_isLeaf;

  OctreeNode* m_parent;
  std::vector<OctreeNodePtr> m_children; // for nodes
  std::vector<BlockRef> m_blocks; // for leafs
};

class Octree
{
public:
    Octree();
    ~Octree();

    void cleanup();

    void build( ShipModel* model );
    void buildNode( OctreeNode* parent, int level );

    const OctreeNode* getRoot() const { return m_root; }
    OctreeNode* getRoot() { return m_root; }


private:

    OctreeNode* m_root;

};

#endif // OCTREE_H
