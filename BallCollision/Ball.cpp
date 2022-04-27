#include "Ball.h"

// constructor
Ball::Ball(float pos_x, float pos_y, float dir_x, float dir_y, float radius) : m_current_position(pos_x, pos_y), m_direction(dir_x, dir_y),
m_radius(radius)
{
}

// integrate movement
void Ball::integrate(float time_delta)
{
	m_prev_position.x = m_current_position.x;
	m_prev_position.y = m_current_position.y;

	float dx = m_direction.x * time_delta;
	float dy = m_direction.y * time_delta;
	m_current_position.x += dx;
	m_current_position.y += dy;
}

// apply x correction
void Ball::correctX(float x_movement)
{
	m_current_position.x += x_movement;
}

// apply y corrention
void Ball::correctY(float y_movement)
{
	m_current_position.y += y_movement;
}

// get x
float Ball::getX() const
{
	return m_current_position.x;
}

// get y
float Ball::getY() const
{
	return m_current_position.y;
}

// get radius
float Ball::getRadius() const
{
	return m_radius;
}

// get mass
float Ball::getMass() const
{
	return m_radius;
}

// get movement x
float Ball::getDirectionX() const
{
	return m_direction.x;
}

// get movement y
float Ball::getDirectionY() const
{
	return m_direction.y;
}

// move ball to the prev position
void Ball::moveToPrevPosition()
{
	m_current_position = m_prev_position;
}

// set movement direction
void Ball::setDirection(float direction_x, float direction_y)
{
	m_direction.x = direction_x;
	m_direction.y = direction_y;
}

// collise with the vertical wall
void Ball::collideVerticalWall(int screen_width)
{
	if (m_current_position.x - m_radius <= 0)
	{
		if (m_direction.x < 0)
		{
			m_direction.x = -m_direction.x;
			// moveToPrevPosition();

			// cout << "Collision between ball and horizontal wall " << endl;
		}
	}

	if (m_current_position.x + m_radius >= screen_width)
	{
		if (m_direction.x > 0)
		{
			m_direction.x = -m_direction.x;
			// moveToPrevPosition();

			// cout << "Collision between ball and horizontal wall " << endl;
		}
	}
}

// collise with the horizontal wall
void Ball::collideHorizontalWall(int screen_height)
{

	if (m_current_position.y - m_radius <= 0)
	{
		if (m_direction.y < 0)
		{
			m_direction.y = -m_direction.y;
			// moveToPrevPosition();

			// cout << "Collision between ball and horizontal wall " << endl;
		}
	}

	if (m_current_position.y + m_radius >= screen_height)
	{
		if (m_direction.y > 0)
		{
			m_direction.y = -m_direction.y;
			// moveToPrevPosition();

			// cout << "Collision between ball and horizontal wall " << endl;
		}
	}
}

// collision between balls
void Ball::ballsCollision(Ball& ball_1, Ball& ball_2)
{
	float ball_1_x = ball_1.getX();
	float ball_1_y = ball_1.getY();
	float ball_2_x = ball_2.getX();
	float ball_2_y = ball_2.getY();

	float vector_between_balls[2];
	vector_between_balls[0] = ball_2_x - ball_1_x;
	vector_between_balls[1] = ball_2_y - ball_1_y;

	float ball_1_radius = ball_1.getRadius();
	float ball_2_radius = ball_2.getRadius();

	float distance_between_balls = hypotf(vector_between_balls[0], vector_between_balls[1]);
	if (distance_between_balls - ball_1_radius - ball_2_radius > 0)
		return;

	float normilized_vector_between_balls[2];
	normilized_vector_between_balls[0] = vector_between_balls[0] / distance_between_balls;
	normilized_vector_between_balls[1] = vector_between_balls[1] / distance_between_balls;

	float balls_positions_corrections[2];
	float optimal_distance = ball_1_radius + ball_2_radius;
	balls_positions_corrections[0] = normilized_vector_between_balls[0] * (optimal_distance - distance_between_balls);
	balls_positions_corrections[1] = normilized_vector_between_balls[1] * (optimal_distance - distance_between_balls);

	float ball_1_mass = ball_1.getMass();
	float ball_2_mass = ball_1.getMass();

	float ball_1_speed_vector[2];
	ball_1_speed_vector[0] = ball_1.getDirectionX();
	ball_1_speed_vector[1] = ball_1.getDirectionY();

	float ball_2_speed_vector[2];
	ball_2_speed_vector[0] = ball_2.getDirectionX();
	ball_2_speed_vector[1] = ball_2.getDirectionY();

	float masses_sum = ball_1_mass + ball_2_mass;
	float masses_detla = ball_1_mass - ball_2_mass;

	float ball_1_new_speed_vector[2];
	ball_1_new_speed_vector[0] = (masses_detla * ball_1_speed_vector[0] + 2 * ball_2_mass * ball_2_speed_vector[0]) / masses_sum;
	ball_1_new_speed_vector[1] = (masses_detla * ball_1_speed_vector[1] + 2 * ball_2_mass * ball_2_speed_vector[1]) / masses_sum;

	float ball_2_new_speed_vector[2];
	ball_2_new_speed_vector[0] = (-masses_detla * ball_2_speed_vector[0] + 2 * ball_1_mass * ball_1_speed_vector[0]) / masses_sum;
	ball_2_new_speed_vector[1] = (-masses_detla * ball_2_speed_vector[1] + 2 * ball_1_mass * ball_1_speed_vector[1]) / masses_sum;

	ball_1.correctX(-balls_positions_corrections[0] * ball_2_mass / masses_sum);
	ball_1.correctY(-balls_positions_corrections[1] * ball_2_mass / masses_sum);

	ball_2.correctX(balls_positions_corrections[0] * ball_1_mass / masses_sum);
	ball_2.correctY(balls_positions_corrections[1] * ball_1_mass / masses_sum);

	ball_1.setDirection(ball_1_new_speed_vector[0], ball_1_new_speed_vector[1]);
	ball_2.setDirection(ball_2_new_speed_vector[0], ball_2_new_speed_vector[1]);

	// cout << "Collision between balls " << endl;
}
