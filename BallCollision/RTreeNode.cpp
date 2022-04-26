#include "RTreeNode.h"

// create  node
RTreeNode::RTreeNode(unsigned node_id, unsigned object_id, bool is_object_node) : m_node_id(node_id), m_object_id(object_id),
m_parent(nullptr), m_is_object_node(is_object_node)
{
}

// get id of object represented by the node
unsigned RTreeNode::getObjectID() const
{
	return m_object_id;
}

// get id of R-tree node
unsigned RTreeNode::getNodeId() const
{
	return m_node_id;
}

// set node parent
void RTreeNode::setParent(RTreeNode* new_parent)
{
	m_parent = new_parent;
}

// get node parent
RTreeNode* RTreeNode::getParent()
{
	return m_parent;
}

// check if node is a leaf (it's children contan objects)
bool RTreeNode::isLeaf() const
{
	if (!m_children.size())
		return false;

	return m_children[0]->isObjectNode();
}

// check if node is an object node (contains some object)
bool RTreeNode::isObjectNode() const
{
	return m_is_object_node;
}

// set bounding box of the node
void RTreeNode::setBndBox(const BndBox& new_bnd_box)
{
	m_bnd_box.copyDataFrom(new_bnd_box);
}

// get bounding box of the node
BndBox RTreeNode::getBndBox() const
{
	return m_bnd_box;
}

// update bounding box of the node
void RTreeNode::recalcBndBox()
{
	for (auto& child : m_children)
		m_bnd_box.addBndBox(child->getBndBox());
}

// set children of the node
void RTreeNode::setChildren(vector<RTreeNode*>& new_children)
{
	m_children = new_children;
}

// get children of the node
vector<RTreeNode*>* RTreeNode::getChildren()
{
	return &m_children;
}

// get copy children array of the node
vector<RTreeNode*> RTreeNode::getChildrenCopy() const
{
	return m_children;
}
