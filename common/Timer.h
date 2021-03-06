#pragma once
#include <windows.h>

/*!
	@file   Timer.h
	@brief  時間計測用のClass
*/

class Timer
{
private:
	LARGE_INTEGER start_;

public:
	Timer()
	{
		Start();
	}

	void Start()
	{
		QueryPerformanceCounter(&start_);
	}
	
	__int64 Elapsed()
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return now.QuadPart - start_.QuadPart;
	}
	
};

