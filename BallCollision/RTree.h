#pragma once
#include <algorithm>
#include <cstdlib>
#include <list>
#include "BndBox.h"
#include "RTreeNode.h"

// r-tree based on Antonin Guttman "R-Trees: A Dynamic Index Structure for Spatial Searching", Proc. 1984

class RTree
{
private:
	unsigned m_max_count;		 // max number of children of the r-tree node
	unsigned m_min_count;		 // min number of children of the r-tree node
	RTreeNode *m_root;			 // root of the node
	unsigned m_node_id_counter;	 // id counter to create id for the new node
	list<RTreeNode *> m_nodes;	 // nodes container

public:
	// here we need min count and max count
	RTree(unsigned min_count, unsigned max_count);

	// insert new object to the r-tree
	void insertObject(float node_x, float node_y, unsigned object_id, float gap);

	// find if some r-tree node collide with the specified point
	vector<unsigned> findCollision(float point_x, float point_y);

	// delete all nodes of the tree and create new root
	void clearTree();

	// delete tree
	~RTree();

private:
	// internal function to instert a node
	void insertNodeInternal(RTreeNode *node_to_insert);

	// choose optimal subtree to insert a node
	RTreeNode *chooseSubtree(RTreeNode *parent_node, RTreeNode *new_node);

	// choose optimal child to insert a node
	unsigned findOptimalChild(RTreeNode *parent_node, RTreeNode *new_node);

	// split node if it has too many children
	void splitNode(RTreeNode *node_to_split, RTreeNode *node_to_insert);

	// find first nodes to put in groups
	void pickSeeds(const vector<RTreeNode *> &nodes, unsigned &initial_node_1,
				   unsigned &initial_node_2, float &group_1_square, float &group_2_square) const;

	// find next node to put in groups
	void findNext(const vector<RTreeNode *> &free_nodes, BndBox group_1_bnd_box, BndBox group_2_bnd_box,
				  float group_1_square_before_addition, float group_2_square_before_addition, unsigned group_1_elem_count,
				  unsigned group_2_elem_count, unsigned &chosen_group, unsigned &chosen_node, float &chosen_group_new_square,
				  BndBox &chosen_group_new_bnd_box) const;
};