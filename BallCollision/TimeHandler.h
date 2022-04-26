#pragma once
#include "SFML/Graphics.hpp"
#include "MiddleAverageFilter.h"

// class to work with time and FPS

class TimeHandler
{
private:
	Math::MiddleAverageFilter<float, 100> m_fps_counter; // object to calc average fps
	sf::Clock m_clock;									 // clock to measure time
	float m_last_time;									 // time of prev frame
	float m_current_time;								 // current time
	float m_time_delta;									 // time delta between frames

public:
	// call before the main loop
	void init();

	// call on new frame
	void frame();

	// get time delta for position integration
	float getTimeDelta() const;

	// get current FPS
	float getFPS() const;
};