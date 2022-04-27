#include <cmath>
#include "SFML/Graphics.hpp"
#include "MiddleAverageFilter.h"
#include "Ball.h"
#include "Output.h"
#include "TimeHandler.h"
#include "RTree.h"

// old parameters
constexpr int WINDOW_X = 1024;
constexpr int WINDOW_Y = 768;
constexpr int MAX_BALLS = 300;
constexpr int MIN_BALLS = 100;

// new parameters
constexpr bool SHOW_FPS = false;			// if we need to show fps
constexpr bool SET_FRAMERATE_LIMIT = false; // if we want to limit fps

// NOTE: there is a rule: 2 <= R_TREE_MIN_COUNT <= R_TREE_MAX_COUNT/2
// NOTE 2: these parameters affect of performance. Now everything is configured fox maximum speed
constexpr unsigned R_TREE_MIN_COUNT = 10; // min number of children in r-tree node
constexpr unsigned R_TREE_MAX_COUNT = 20; // max number of children in r-tree node

int main()
{
	// create a window
	sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "ball collision demo");

	// create an output object
	Output output(window, SHOW_FPS);
	if (SET_FRAMERATE_LIMIT)
		output.setFrametateLimit(60);

	// create vector of balls
	srand(time(NULL));
	unsigned balls_count = (rand() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS);
	std::vector<Ball> balls;
	balls.reserve(balls_count);

	// generate balls data
	float ball_x, ball_y, ball_radius, ball_dir_x, ball_dir_y, ball_speed;
	for (int i = 0; i < balls_count; i++)
	{
		ball_x = rand() % WINDOW_X;
		ball_y = rand() % WINDOW_Y;
		ball_speed = 30 + rand() % 30;
		ball_dir_x = (rand() % 1000 / 1000.);
		ball_dir_y = ((rand() % 2) - 1) * sqrtf(1 - ball_dir_x * ball_dir_x);
		ball_dir_x *= ball_speed;
		ball_dir_y *= ball_speed;
		ball_radius = 5 + rand() % 5;
		balls.push_back(Ball(ball_x, ball_y, ball_dir_x, ball_dir_y, ball_radius));
	}

	// create a time handler
	TimeHandler time_handler;
	time_handler.init();

	// create r-tree
	RTree r_tree(R_TREE_MIN_COUNT, R_TREE_MAX_COUNT);

	// main loop
	float time_delta;
	while (window.isOpen())
	{
		// handle close event if it exists
		sf::Event event;
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
			{
				output.closeWindow();
				break;
			}

		// record data about a new frame
		time_handler.frame();
		time_delta = time_handler.getTimeDelta();

		// move all balls
		for (auto &ball : balls)
			ball.integrate(time_delta);

		// put new data to the r-tree
		r_tree.clearTree();
		for (unsigned i = 0; i < balls.size(); ++i)
			r_tree.insertObject(balls[i].getX(), balls[i].getY(), i, balls[i].getRadius() * 2);

		// check collisions with screen borders
		for (auto &ball : balls)
		{
			ball.collideVerticalWall(WINDOW_X);
			ball.collideHorizontalWall(WINDOW_Y);
		}

		// find collisions between balls
		vector<unsigned> collided_balls;
		for (unsigned i = 0; i < balls.size(); ++i)
		{
			collided_balls = r_tree.findCollision(balls[i].getX(), balls[i].getY());

			if (!collided_balls.size())
				throw new exception("R-tree error");

			for (auto &collided_ball : collided_balls)
				if (collided_ball != i)
					Ball::ballsCollision(balls[i], balls[collided_ball]);
		}

		// draw a new frame
		window.clear();
		output.drawBalls(balls);
		output.display(time_handler.getFPS());
	}

	return 0;
}