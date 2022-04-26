#include "BndBox.h"

// constructor
BndBox::BndBox()
{
	setVoid();
}

// set void state
void BndBox::setVoid()
{
	m_data[0] = FLT_MAX;
	m_data[1] = -FLT_MAX;
	m_data[2] = FLT_MAX;
	m_data[3] = -FLT_MAX;

	m_data[4] = 0;
}

// add vertex
void BndBox::setVertex(float vertex_x, float vertex_y, float gap)
{
	m_data[0] = vertex_x - gap;
	m_data[1] = vertex_x + gap;
	m_data[2] = vertex_y - gap;
	m_data[3] = vertex_y + gap;

	m_data[4] = gap;
}

// copy data between bounding boxes
void BndBox::copyDataFrom(const BndBox &source)
{
	m_data[0] = source.m_data[0];
	m_data[1] = source.m_data[1];
	m_data[2] = source.m_data[2];
	m_data[3] = source.m_data[3];
	m_data[4] = source.m_data[4];
}

// add bounding box
void BndBox::addBndBox(const BndBox &bnd_box)
{
	if (bnd_box.m_data[0] < m_data[0])
		m_data[0] = bnd_box.m_data[0];
	if (bnd_box.m_data[1] > m_data[1])
		m_data[1] = bnd_box.m_data[1];

	if (bnd_box.m_data[2] < m_data[2])
		m_data[2] = bnd_box.m_data[2];
	if (bnd_box.m_data[3] > m_data[3])
		m_data[3] = bnd_box.m_data[3];
}

// check if point is inside the bounding box
bool BndBox::contains(float point_x, float point_y) const
{
	bool is_inside = true;

	is_inside &= point_x > m_data[0];
	is_inside &= point_x < m_data[1];
	is_inside &= point_y > m_data[2];
	is_inside &= point_y < m_data[3];

	return is_inside;
}

// get gap
float BndBox::getGap() const
{
	return m_data[4];
}

// get square
float BndBox::getSquare() const
{
	float dimension_1 = (m_data[1] - m_data[0]);
	float dimension_2 = (m_data[3] - m_data[2]);

	if (!dimension_1 || !dimension_2)
		return 0;

	return dimension_1 * dimension_2;
}

// get perimeter
float BndBox::getPerimeter() const
{
	float dimension_1 = (m_data[1] - m_data[0]);
	float dimension_2 = (m_data[3] - m_data[2]);

	return dimension_1 + dimension_1 + dimension_2 + dimension_2;
}

// calculate overlap
float BndBox::calcBndBoxesOverlap(const BndBox &box_a, const BndBox &box_b)
{
	float overlap_data[6];
	float overlap_dimensions[2];

	overlap_data[0] = fmax(box_a.m_data[0], box_b.m_data[0]);
	overlap_data[1] = fmin(box_a.m_data[1], box_b.m_data[1]);
	overlap_data[2] = fmin(box_a.m_data[2], box_b.m_data[2]);
	overlap_data[3] = fmax(box_a.m_data[3], box_b.m_data[3]);

	overlap_dimensions[0] = (overlap_data[1] - overlap_data[0]);
	overlap_dimensions[1] = (overlap_data[3] - overlap_data[2]);

	if (overlap_dimensions[0] < 0 || overlap_dimensions[1] < 0)
		return 0;

	return overlap_dimensions[0] * overlap_dimensions[1];
}
