#pragma once
#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	float GetDeltaTime( bool capAt60FPS = true );
	inline bool IsCappedAt60() const noexcept { return m_bCapAt60FPS; }
private:
	bool m_bCapAt60FPS = false;
};

#endif