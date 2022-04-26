#pragma once
#include "Output.h"

// constructor
Output::Output(sf::RenderWindow &window, bool need_display_fps = false) : m_window(window), m_need_display_fps(m_need_display_fps)
{
	m_framerate_limit = UINT_MAX;
}

// if we need to limit fps
void Output::setFrametateLimit(unsigned limit)
{
	m_framerate_limit = limit;
	m_window.setFramerateLimit(m_framerate_limit);
}

// callback to record data about balls
void Output::drawBalls(const std::vector<Ball> &balls)
{
	sf::CircleShape gball;

	for (auto &ball : balls)
	{
		gball.setRadius(ball.getRadius());
		gball.setPosition(ball.getX(), ball.getY());
		m_window.draw(gball);
	}
}

// refresh screen
void Output::display(float fps)
{
	if (m_need_display_fps)
	{
		char c[32];
		snprintf(c, 32, "FPS: %f", fps);
		std::string string(c);
		sf::String str(c);
		m_window.setTitle(str);
	}

	m_window.display();
}

// handle closed window event
void Output::closeWindow()
{
	m_window.close();
}