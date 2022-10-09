#pragma once

class Timer
{
public:
	Timer();

	// Returns time elapsed since last Reset() call
	float TotalTime() const;
	// Return frame interval
	float DeltaTime() const { return static_cast<float>(m_deltaTime); };

	// Called before the timing starts or when a reset is required
	void Reset();
	// Called when timinig starts or when pause is cancelled
	void Start();
	// Called when pause is required
	void Stop();
	// Called at the beginning of each frame
	void Tick();

	// Returns if the timer is paused/stopped
	bool IsStopped() const { return m_stopped; }

	operator float() const { return static_cast<float>(m_deltaTime); }

private:
	double m_secondsPerCount = 0.0f;
	double m_deltaTime = -1.0f;

	__int64 m_baseTime = 0;
	__int64 m_pausedTime = 0;
	__int64 m_stopTime = 0;
	__int64 m_prevTime = 0;
	__int64 m_currTime = 0;

	bool m_stopped = false;
};