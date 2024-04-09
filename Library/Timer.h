#pragma once

#include <Windows.h>

class Timer
{
private:
	Timer()
	{
		LONGLONG counts_per_sec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		secondsPerCount = 1.0 / static_cast<double>(counts_per_sec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;
	}
	~Timer() = default;
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer(Timer&&) noexcept = delete;
	Timer& operator=(Timer&&) noexcept = delete;

public:
	static Timer& Instance()
	{
		static Timer instance;
		return instance;
	}

private:
	double secondsPerCount{ 0.0 };
	double deltaTime{ 0.0 };

	LONGLONG baseTime{ 0LL };
	LONGLONG pausedTime{ 0LL };
	LONGLONG stopTime{ 0LL };
	LONGLONG lastTime{ 0LL };
	LONGLONG thisTime{ 0LL };

	bool stopped{ false };

public:

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	float TimeStamp() const  // in seconds
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// stopTime - baseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--pausedTime-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  baseTime       stopTime        start_time     stopTime    thisTime

		if (stopped)
		{
			return static_cast<float>(((stopTime - pausedTime) - baseTime) * secondsPerCount);
		}

		// The distance thisTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from thisTime:  
		//
		//  (thisTime - pausedTime) - baseTime 
		//
		//                     |<--pausedTime-->|
		// ----*---------------*-----------------*------------*------> time
		//  baseTime       stopTime        start_time     thisTime
		else
		{
			return static_cast<float>(((thisTime - pausedTime) - baseTime) * secondsPerCount);
		}
	}

	float TimeInterval() const  // in seconds
	{
		return static_cast<float>(deltaTime);
	}

	void Reset() // Call before message loop.
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;

		stopTime = 0;
		stopped = false;
	}

	void Start() // Call when unpaused.
	{
		LONGLONG start_time;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  baseTime       stopTime        start_time     
		if (stopped)
		{
			pausedTime += (start_time - stopTime);
			lastTime = start_time;
			stopTime = 0;
			stopped = false;
		}
	}

	void Stop() // Call when paused.
	{
		if (!stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stopTime));
			stopped = true;
		}
	}

	void Tick() // Call every frame.
	{
		if (stopped)
		{
			deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		// Time difference between this frame and the previous.
		deltaTime = (thisTime - lastTime) * secondsPerCount;

		// Prepare for next frame.
		lastTime = thisTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (deltaTime < 0.0)
		{
			deltaTime = 0.0;
		}
	}

	// --- �Q�b�^�[ ---
	float DeltaTime() const
	{
		return static_cast<float>(deltaTime);
	}
};