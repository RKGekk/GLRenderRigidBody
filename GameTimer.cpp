#include "GameTimer.h"

GameTimer::GameTimer() : _secondsPerCount(0.0), _deltaTime(-1.0), _baseTime(0), _pausedTime(0), _stopTime(0), _prevousTime(0), _currentTime(0), _isStopped(false) {
	__int64 countPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER *)&countPerSec);
	_secondsPerCount = 1.0 / ((double)countPerSec);
}

float GameTimer::gameTime() const {
	if (_isStopped) {
		return (float)(((_stopTime - _pausedTime) - _baseTime) * _secondsPerCount);
	}
	else {
		return (float)(((_currentTime - _pausedTime) - _baseTime) * _secondsPerCount);
	}
}

float GameTimer::deltaTime() const {
	return (float)_deltaTime;
}

void GameTimer::reset() {
	QueryPerformanceCounter((LARGE_INTEGER *)&_currentTime);
	_baseTime = _currentTime;
	_prevousTime = _currentTime;
	_stopTime = 0;
	_isStopped = false;
}

void GameTimer::start() {
	QueryPerformanceCounter((LARGE_INTEGER *)&_currentTime);
	if (_isStopped) {
		_pausedTime += (_currentTime - _stopTime);
		_prevousTime = _currentTime;
		_stopTime = 0;
		_isStopped = false;
	}
}

void GameTimer::stop() {
	if (!_isStopped) {
		QueryPerformanceCounter((LARGE_INTEGER *)&_currentTime);
		_stopTime = _currentTime;
		_isStopped = true;
	}
}

void GameTimer::tick() {
	if (_isStopped) {
		_deltaTime = 0.0;
		return;
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&_currentTime);
	_deltaTime = (_currentTime - _prevousTime) * _secondsPerCount;
	_prevousTime = _currentTime;
	if (_deltaTime < 0.0) {
		_deltaTime = 0.0;
	}
};