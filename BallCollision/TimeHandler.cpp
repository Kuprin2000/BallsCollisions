#include "TimeHandler.h"

// call before the main loop
void TimeHandler::init()
{
	m_last_time = m_clock.restart().asSeconds();
}

// call on new frame
void TimeHandler::frame()
{
	m_current_time = m_clock.getElapsedTime().asSeconds();
	m_time_delta = m_current_time - m_last_time;
	m_fps_counter.push(1.0f / (m_current_time - m_last_time));
	m_last_time = m_current_time;
}

// get time delta for position integration
float TimeHandler::getTimeDelta() const
{
	return m_time_delta;
}

// get current FPS

float TimeHandler::getFPS() const
{
	return m_fps_counter.getAverage();
}
