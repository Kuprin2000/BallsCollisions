#pragma once
#include <vector>
#include "BndBox.h"

using namespace std;

// node of r-tree

class RTreeNode
{
private:
	unsigned m_node_id;	   // id of R-tree node
	unsigned m_object_id;  // id of object represented by the node
	bool m_is_object_node; // of node contains some object

	vector<RTreeNode*> m_children; // children of the node
	RTreeNode* m_parent;			// parent of the node

	BndBox m_bnd_box; // bounding box of the node

public:
	// create  node
	RTreeNode(unsigned node_id, unsigned object_id, bool is_object_node);

	// get id of object represented by the node
	unsigned getObjectID() const;

	// get id of R-tree node
	unsigned getNodeId() const;

	// set node parent
	void setParent(RTreeNode* new_parent);

	// get node parent
	RTreeNode* getParent();

	// check if node is a leaf (it's children contan objects)
	bool isLeaf() const;

	// check if node is an object node (contains some object)
	bool isObjectNode() const;

	// set bounding box of the node
	void setBndBox(const BndBox& new_bnd_box);

	// get bounding box of the node
	BndBox getBndBox() const;

	// update bounding box of the node
	void recalcBndBox();

	// set children of the node
	void setChildren(vector<RTreeNode*>& new_children);

	// get children of the node
	vector<RTreeNode*>* getChildren();

	// get copy children array of the node
	vector<RTreeNode*> getChildrenCopy() const;
};