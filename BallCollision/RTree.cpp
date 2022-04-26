#pragma once
#include "RTree.h"

// here we need min count and max count
RTree::RTree(unsigned min_count, unsigned max_count) : m_min_count(min_count), m_max_count(max_count), m_node_id_counter(1)
{
	m_root = new RTreeNode(m_node_id_counter, -1, false);
	m_nodes.push_back(m_root);
}

// insert new object to the r-tree
void RTree::insertObject(float node_x, float node_y, unsigned object_id, float gap)
{
	// create a new node
	RTreeNode *new_node = new RTreeNode(m_node_id_counter, object_id, true);
	m_nodes.push_back(new_node);
	++m_node_id_counter;

	// create bounding box
	BndBox new_node_bnd_box;
	new_node_bnd_box.setVertex(node_x, node_y, gap);
	new_node->setBndBox(new_node_bnd_box);

	// call internal function
	insertNodeInternal(new_node);
}

// find if some r-tree node collide with the specified point
vector<unsigned> RTree::findCollision(float point_x, float point_y)
{
	RTreeNode *current_node;
	vector<RTreeNode *> *current_node_children;

	// create a stack of candidates
	vector<RTreeNode *> candidates_stack;
	candidates_stack.push_back(m_root);

	vector<unsigned> result;

	// check candidates until stack is not empty
	while (candidates_stack.size())
	{
		current_node = *(--candidates_stack.end());
		candidates_stack.pop_back();

		current_node_children = current_node->getChildren();

		if (current_node->isLeaf())
		{
			for (auto &child : *current_node_children)
				if (child->getBndBox().contains(point_x, point_y))
					result.push_back(child->getObjectID());
		}

		else
		{
			for (auto &child : *current_node_children)
				if (child->getBndBox().contains(point_x, point_y))
					candidates_stack.push_back(child);
		}
	}

	return result;
}

// delete all nodes of the tree and create new root
void RTree::clearTree()
{
	// here we use m_nodes container
	for (auto &node : m_nodes)
		delete (node);

	// create a new root
	m_nodes.clear();
	m_root = new RTreeNode(0, -1, false);
	m_nodes.push_back(m_root);
	m_node_id_counter = 1;
}

// delete tree
RTree::~RTree()
{
	for (auto &node : m_nodes)
		delete (node);
}

// internal function to instert a node
void RTree::insertNodeInternal(RTreeNode *node_to_insert)
{
	// find optimal leaf to push into
	RTreeNode *node_to_push_into = chooseSubtree(m_root, node_to_insert);

	// if leaf has appropriate number of children simply add a new child
	if (node_to_push_into->getChildren()->size() < m_max_count)
	{
		node_to_insert->setParent(node_to_push_into);
		node_to_push_into->getChildren()->push_back(node_to_insert);

		for (RTreeNode *node = node_to_push_into; node != nullptr; node = node->getParent())
			node->recalcBndBox();
	}

	// or we have to split node
	else
		splitNode(node_to_push_into, node_to_insert);
}

// choose optimal subtree to insert a node
RTreeNode *RTree::chooseSubtree(RTreeNode *parent_node, RTreeNode *new_node)
{
	RTreeNode *current_node = parent_node;
	unsigned optimal_child;

	// recursively look for a new child
	while (true)
	{
		// return an optimal leaf when it is found
		if (current_node->isLeaf() || (current_node == m_root && !current_node->getChildren()->size()))
			return current_node;

		optimal_child = findOptimalChild(current_node, new_node);

		current_node = current_node->getChildren()->at(optimal_child);
	}
}

// choose optimal child to insert a node
unsigned RTree::findOptimalChild(RTreeNode *parent_node, RTreeNode *new_node)
{
	if (parent_node->isLeaf() || !parent_node->getChildren()->size())
		return UINT_MAX;

	vector<RTreeNode *> *nodes = parent_node->getChildren();

	BndBox node_bnd_box;

	// firstly calc squares of all children before and after addition of new node
	// we also need square deltas
	float square_before_addition;
	float square_after_addition;
	vector<float> squares_after_addition(nodes->size());
	vector<float> square_deltas(nodes->size());

	vector<unsigned> candidates(nodes->size());
	unsigned number_of_candidates;

	for (unsigned i = 0; i < nodes->size(); ++i)
		candidates[i] = i;

	for (unsigned i = 0; i < nodes->size(); ++i)
	{
		node_bnd_box.copyDataFrom(nodes->at(i)->getBndBox());

		square_before_addition = node_bnd_box.getSquare();

		node_bnd_box.addBndBox(new_node->getBndBox());

		square_after_addition = node_bnd_box.getSquare();
		squares_after_addition[i] = square_after_addition;
		square_deltas[i] = square_after_addition - square_before_addition;
	}

	// find candidates with minimum square deltas
	auto min_square_delta_iter = min_element(square_deltas.begin(), square_deltas.end());
	float min_square_delta = *min_square_delta_iter;
	number_of_candidates = 0;
	for (unsigned i = 0; i < square_deltas.size(); ++i)
		if (square_deltas[i] == min_square_delta)
			++number_of_candidates;

	// if there is only one return it
	if (number_of_candidates == 1)
		return distance(square_deltas.begin(), min_square_delta_iter);

	// of mark all other candidates as bad
	for (unsigned i = 0; i < candidates.size(); ++i)
		if (square_deltas[i] != min_square_delta)
			squares_after_addition[i] = FLT_MAX;

	// return candidate with the least square
	auto min_square_iter = min_element(squares_after_addition.begin(), squares_after_addition.end());
	return distance(squares_after_addition.begin(), min_square_iter);
}

// split node if it has too many children
void RTree::splitNode(RTreeNode *node_to_split, RTreeNode *node_to_insert)
{
	vector<RTreeNode *> free_nodes = node_to_split->getChildrenCopy();
	free_nodes.push_back(node_to_insert);

	// we will divide children to two groups
	vector<RTreeNode *> group_1;
	vector<RTreeNode *> group_2;

	unsigned initial_node_1;
	unsigned initial_node_2;
	float group_1_square;
	float group_2_square;

	unsigned chosen_group;
	unsigned chosen_node;
	float chosen_group_new_square;
	BndBox chosen_group_new_bnd_box;

	int must_add_to_group_1;
	int must_add_to_group_2;
	int must_add_to_groups_count;

	bool need_to_recalc_bnd_box = false;

	// find initial elements of two groups
	pickSeeds(free_nodes, initial_node_1, initial_node_2, group_1_square, group_2_square);

	group_1.push_back(free_nodes[initial_node_1]);
	group_2.push_back(free_nodes[initial_node_2]);

	BndBox group_1_bnd_box;
	group_1_bnd_box.copyDataFrom(free_nodes[initial_node_1]->getBndBox());
	BndBox group_2_bnd_box;
	group_2_bnd_box.copyDataFrom(free_nodes[initial_node_2]->getBndBox());

	// delete initial elements from array of free elements
	if (initial_node_1 < initial_node_2)
	{
		free_nodes.erase(free_nodes.begin() + initial_node_2);
		free_nodes.erase(free_nodes.begin() + initial_node_1);
	}
	else
	{
		free_nodes.erase(free_nodes.begin() + initial_node_1);
		free_nodes.erase(free_nodes.begin() + initial_node_2);
	}

	// while there are fre nodes left
	while (free_nodes.size())
	{
		// check if groups are going to have right sizes
		must_add_to_group_1 = m_min_count - group_1.size();
		if (must_add_to_group_1 < 0)
			must_add_to_group_1 = 0;
		must_add_to_group_2 = m_min_count - group_2.size();
		if (must_add_to_group_2 < 0)
			must_add_to_group_2 = 0;

		must_add_to_groups_count = must_add_to_group_1 + must_add_to_group_2;
		if (must_add_to_groups_count == free_nodes.size())
		{
			for (unsigned i = 0; i < must_add_to_group_1; ++i)
				group_1.push_back(free_nodes[i]);

			for (unsigned i = must_add_to_group_1; i < must_add_to_groups_count; ++i)
				group_2.push_back(free_nodes[i]);

			free_nodes.clear();
			need_to_recalc_bnd_box = true;
			continue;
		}

		// find next free node to place to group
		findNext(free_nodes, group_1_bnd_box, group_2_bnd_box, group_1_square, group_2_square, group_1.size(), group_2.size(), chosen_group, chosen_node, chosen_group_new_square, chosen_group_new_bnd_box);

		// place it to the group and remove from free nodes array
		if (chosen_group == 1)
		{
			group_1.push_back(free_nodes[chosen_node]);
			group_1_bnd_box.copyDataFrom(chosen_group_new_bnd_box);
			group_1_square = chosen_group_new_square;
		}

		else
		{
			group_2.push_back(free_nodes[chosen_node]);
			group_2_bnd_box.copyDataFrom(chosen_group_new_bnd_box);
			group_2_square = chosen_group_new_square;
		}

		free_nodes.erase(free_nodes.begin() + chosen_node);
	}

	// create new nodes
	RTreeNode *first_new_node = new RTreeNode(m_node_id_counter, -1, false);
	RTreeNode *second_new_node = new RTreeNode(m_node_id_counter + 1, -1, false);
	m_nodes.push_back(first_new_node);
	m_nodes.push_back(second_new_node);
	m_node_id_counter += 2;

	first_new_node->setChildren(group_1);
	for (unsigned i = 0; i < first_new_node->getChildren()->size(); ++i)
		first_new_node->getChildren()->at(i)->setParent(first_new_node);

	if (need_to_recalc_bnd_box)
		first_new_node->recalcBndBox();
	else
		first_new_node->setBndBox(group_1_bnd_box);

	second_new_node->setChildren(group_2);
	for (unsigned i = 0; i < second_new_node->getChildren()->size(); ++i)
		second_new_node->getChildren()->at(i)->setParent(second_new_node);

	if (need_to_recalc_bnd_box)
		second_new_node->recalcBndBox();
	else
		second_new_node->setBndBox(group_2_bnd_box);

	// update parent of two new nodes
	vector<RTreeNode *> *children_array_to_update;
	if (node_to_split->getParent() != nullptr)
	{
		first_new_node->setParent(node_to_split->getParent());
		second_new_node->setParent(node_to_split->getParent());

		// push first of two nodes to the children array of the parent
		children_array_to_update = node_to_split->getParent()->getChildren();
		for (unsigned i = 0; i < children_array_to_update->size(); ++i)
			if (children_array_to_update->at(i)->getNodeId() == node_to_split->getNodeId())
				children_array_to_update->at(i) = first_new_node;

		// may be we have to split a parent
		if (children_array_to_update->size() < m_max_count)
		{
			children_array_to_update->push_back(second_new_node);
		}
		else
		{
			for (RTreeNode *node = node_to_split->getParent(); node != nullptr; node = node->getParent())
				node->recalcBndBox();

			splitNode(node_to_split->getParent(), second_new_node);
		}

		// recalc bounding boxes
		for (RTreeNode *node = second_new_node->getParent(); node != nullptr; node = node->getParent())
			node->recalcBndBox();
	}

	// may be we have to create a new parent
	else
	{
		RTreeNode *new_root = new RTreeNode(m_node_id_counter, -1, false);
		m_nodes.push_back(new_root);
		++m_node_id_counter;

		vector<RTreeNode *> new_children = {first_new_node, second_new_node};
		new_root->setChildren(new_children);

		m_root = new_root;
		first_new_node->setParent(m_root);
		second_new_node->setParent(m_root);

		m_root->recalcBndBox();
	}
}

// find first nodes to put in groups
void RTree::pickSeeds(const vector<RTreeNode *> &nodes, unsigned &initial_node_1, unsigned &initial_node_2, float &group_1_square, float &group_2_square) const
{
	RTreeNode *node_1;
	RTreeNode *node_2;
	float node_1_square;
	float node_2_square;
	BndBox sum_of_bnd_boxes;

	float current_d;
	float min_d = FLT_MAX;
	unsigned optimal_node_1;
	unsigned optimal_node_2;
	float optimal_node_1_square;
	float optimal_node_2_square;

	// we are looking for pair of nodes that will be optimal to put in different groups
	// out criteria is to mimimize (sum of bounding boxes square - node 1 square - node 2 square)->min
	for (unsigned i = 0; i < nodes.size(); ++i)
		for (unsigned j = i + 1; j < nodes.size(); ++j)
		{
			node_1 = nodes[i];
			node_2 = nodes[j];
			node_1_square = node_1->getBndBox().getSquare();
			node_2_square = node_2->getBndBox().getSquare();
			sum_of_bnd_boxes.copyDataFrom(node_1->getBndBox());
			sum_of_bnd_boxes.addBndBox(node_2->getBndBox());

			current_d = sum_of_bnd_boxes.getSquare() - node_1_square - node_2_square;
			if (current_d < min_d)
			{
				min_d = current_d;
				optimal_node_1 = i;
				optimal_node_2 = j;
				optimal_node_1_square = node_1_square;
				optimal_node_2_square = node_2_square;
			}
		}

	initial_node_1 = optimal_node_1;
	initial_node_2 = optimal_node_2;
	group_1_square = optimal_node_1_square;
	group_2_square = optimal_node_2_square;
}

// find next nodes to put in groups
void RTree::findNext(const vector<RTreeNode *> &free_nodes, BndBox group_1_bnd_box, BndBox group_2_bnd_box,
					 float group_1_square_before_addition, float group_2_square_before_addition, unsigned group_1_elem_count,
					 unsigned group_2_elem_count, unsigned &chosen_group, unsigned &chosen_node, float &chosen_group_new_square,
					 BndBox &chosen_group_new_bnd_box) const
{

	RTreeNode *free_node;
	BndBox group_1_bnd_box_after_addition;
	BndBox group_2_bnd_box_after_addition;

	float group_1_square_after_addition;
	float group_2_square_after_addition;

	float group_1_square_delta;
	float group_2_square_delta;

	float square_delta;
	float max_square_delta = -FLT_MAX;
	unsigned optimal_node;
	float optimal_group_1_square_after_addition;
	float optimal_group_2_square_after_addition;
	float optimal_group_1_square_delta;
	float optimal_group_2_square_delta;
	BndBox optimal_group_1_bnd_box_after_addition;
	BndBox optimal_group_2_bnd_box_after_addition;

	// we are looking a new node to put in one of the groups
	// out criteria is to mimimize (group 1 square delta - group 2 square delta)->min
	for (unsigned i = 0; i < free_nodes.size(); ++i)
	{
		free_node = free_nodes[i];
		group_1_bnd_box_after_addition.copyDataFrom(group_1_bnd_box);
		group_1_bnd_box_after_addition.addBndBox(free_node->getBndBox());
		group_2_bnd_box_after_addition.copyDataFrom(group_2_bnd_box);
		group_2_bnd_box_after_addition.addBndBox(free_node->getBndBox());

		group_1_square_after_addition = group_1_bnd_box_after_addition.getSquare();
		group_2_square_after_addition = group_2_bnd_box_after_addition.getSquare();

		group_1_square_delta = group_1_square_after_addition - group_1_square_before_addition;
		group_2_square_delta = group_2_square_after_addition - group_2_square_before_addition;

		square_delta = fabs(group_1_square_delta - group_2_square_delta);

		if (square_delta > max_square_delta)
		{
			max_square_delta = square_delta;
			optimal_node = i;
			optimal_group_1_square_after_addition = group_1_square_after_addition;
			optimal_group_2_square_after_addition = group_2_square_after_addition;
			optimal_group_1_square_delta = group_1_square_delta;
			optimal_group_2_square_delta = group_2_square_delta;
			optimal_group_1_bnd_box_after_addition.copyDataFrom(group_1_bnd_box_after_addition);
			optimal_group_2_bnd_box_after_addition.copyDataFrom(group_2_bnd_box_after_addition);
		}
	}

	// if one of the groups have smaller square delta
	if (optimal_group_1_square_delta < optimal_group_2_square_delta)
	{
		chosen_group = 1;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_1_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_1_bnd_box_after_addition;
		return;
	}

	if (optimal_group_1_square_delta > optimal_group_2_square_delta)
	{
		chosen_group = 2;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_2_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_2_bnd_box_after_addition;
		return;
	}

	// if square deltas are equal chose group with the least square
	if (group_1_square_before_addition < group_2_square_before_addition)
	{
		chosen_group = 1;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_1_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_1_bnd_box_after_addition;
		return;
	}

	if (group_1_square_before_addition > group_2_square_before_addition)
	{
		chosen_group = 2;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_2_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_2_bnd_box_after_addition;
		return;
	}

	// if squares are equal chose group with the least elements count
	if (group_1_elem_count < group_2_elem_count)
	{
		chosen_group = 1;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_1_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_1_bnd_box_after_addition;
		return;
	}

	if (group_1_elem_count > group_2_elem_count)
	{
		chosen_group = 2;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_2_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_2_bnd_box_after_addition;
		return;
	}

	// if the least elements count are equal chose random group
	if (rand() % 2)
	{
		chosen_group = 1;
		chosen_node = optimal_node;
		chosen_group_new_square = optimal_group_1_square_after_addition;
		chosen_group_new_bnd_box = optimal_group_1_bnd_box_after_addition;
		return;
	}

	chosen_group = 2;
	chosen_node = optimal_node;
	chosen_group_new_square = optimal_group_2_square_after_addition;
	chosen_group_new_bnd_box = optimal_group_2_bnd_box_after_addition;
}