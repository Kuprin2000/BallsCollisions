#pragma once
#include "Ball.h"
#include "SFML/Graphics.hpp"

// class to handle output

class Output
{
private:
	sf::RenderWindow &m_window;
	bool m_need_display_fps;
	unsigned m_framerate_limit;

public:
	// constructor
	Output(sf::RenderWindow &window, bool need_display_fps);

	// if we need to limit fps
	void setFrametateLimit(unsigned limit);

	// callback to record data about balls
	void drawBalls(const std::vector<Ball> &balls);

	// refresh screen
	void display(float fps);

	// handle closed window event
	void closeWindow();
};