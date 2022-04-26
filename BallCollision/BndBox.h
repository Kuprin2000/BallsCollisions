#pragma once
#include <cmath>
#include <cfloat>

// axis alined bounding box

class BndBox
{
private:
	float m_data[5]; // data storage format x_min, x_max, y_min, y_max,gap
	bool m_is_void;	 // if bounding box is void

public:
	// constructor
	BndBox();

	// set void state
	void setVoid();

	// add vertex
	void setVertex(float vertex_x, float vertex_y, float gap);

	// copy data between bounding boxes
	void copyDataFrom(const BndBox &source);

	// add bounding box
	void addBndBox(const BndBox &bnd_box);

	// check if point is inside the bounding box
	bool contains(float point_x, float point_y) const;

	// get gap
	float getGap() const;

	// get square
	float getSquare() const;

	// get perimeter
	float getPerimeter() const;

	// calculate overlap
	static float calcBndBoxesOverlap(const BndBox &box_a, const BndBox &box_b);
};
