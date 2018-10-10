#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <windows.h>

class GameTimer {

private:
	double _secondsPerCount;
	double _deltaTime;

	__int64 _baseTime;
	__int64 _pausedTime;
	__int64 _stopTime;
	__int64 _prevousTime;
	__int64 _currentTime;

	bool _isStopped;

public:
	GameTimer();
	float gameTime() const;
	float deltaTime() const;
	void reset();
	void start();
	void stop();
	void tick();
};

#endif
