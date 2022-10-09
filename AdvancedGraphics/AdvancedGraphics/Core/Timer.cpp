#include "pch.h"
#include "Timer.h"

Timer::Timer()
{
	__int64 countsPerSec{};
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

void Timer::Reset()
{
	__int64 currTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_pausedTime = 0; 	// NOTE: If multiple resets are involved, it should be set to 0
	m_stopped = false;
}

void Timer::Stop()
{
	if (!m_stopped)
	{
		__int64 currTime{};
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_stopTime = currTime;
		m_stopped = true;
	}
}

void Timer::Start()
{
	__int64 startTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_stopped)
	{
		m_pausedTime += (startTime - m_stopTime);

		m_prevTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void Timer::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	//The frame interval between the current tick and the previous tick
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	m_prevTime = m_currTime;

	if (m_deltaTime < 0.0)
		m_deltaTime = 0.0;
}

float Timer::TotalTime()const
{
	if (m_stopped)
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	else
		return (float)(((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
}