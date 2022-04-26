#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <cmath>

// ball class

using namespace std;

class Ball
{
private:
	sf::Vector2f m_prev_position;	 // previous position of the ball
	sf::Vector2f m_current_position; // current position of the ball
	sf::Vector2f m_direction;		 // movement vector of the ball (length is the speed)
	float m_radius;					 // radius of the ball

public:
	// constructor
	Ball(float pos_x, float pos_y, float dir_x, float dir_y, float radius);

	// integrate movement
	void integrate(float time_delta);

	// apply x correction
	void correctX(float x_movement);

	// apply y corrention
	void correctY(float y_movement);

	// get x
	float getX() const;

	// get y
	float getY() const;

	// get radius
	float getRadius() const;

	// get mass
	float getMass() const;

	// set movement direction
	void setDirection(float direction_x, float direction_y);

	// get movement x
	float getDirectionX() const;

	// get movement y
	float getDirectionY() const;

	// move ball to the prev position
	void moveToPrevPosition();

	// collise with the vertical wall
	void collideVerticalWall(int screen_width);

	// collise with the horizontal wall
	void collideHorizontalWall(int screen_height);

	// collision between balls
	static void ballsCollision(Ball& ball_1, Ball& ball_2);
};
