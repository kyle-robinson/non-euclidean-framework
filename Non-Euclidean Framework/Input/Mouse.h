#pragma once
#ifndef MOUSE_H
#define MOUSE_H

#include <queue>

struct MousePoint
{
	int x, y;
};

class Mouse
{
public:
	class MouseEvent
	{
	public:
		enum EventType
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			WheelUp,
			WheelDown,
			Move,
			RawMove,
			Enter,
			Leave,
			Invalid
		};
	public:
		MouseEvent();
		MouseEvent( const EventType type, const int x, const int y );
		bool IsValid() const noexcept;
		EventType GetType() const noexcept;
		MousePoint GetPos() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
	private:
		EventType type;
		int x, y;
	};
public:
	void OnLeftPressed( int x, int y ) noexcept;
	void OnLeftReleased( int x, int y ) noexcept;
	void OnRightPressed( int x, int y ) noexcept;
	void OnRightReleased( int x, int y ) noexcept;
	void OnMiddlePressed( int x, int y ) noexcept;
	void OnMiddleReleased( int x, int y ) noexcept;
	void OnWheelUp( int x, int y ) noexcept;
	void OnWheelDown( int x, int y ) noexcept;
	void OnMouseMove( int x, int y ) noexcept;
	void OnMouseMoveRaw( int x, int y ) noexcept;
	void OnMouseEnter( int x, int y ) noexcept;
	void OnMouseLeave( int x, int y ) noexcept;
public:
	bool IsLeftDown() const noexcept;
	bool IsRightDown() const noexcept;
	bool IsMiddleDown() const noexcept;
	bool IsInWindow() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	MousePoint GetPos() const noexcept;
	bool EventBufferIsEmpty() const noexcept;
	MouseEvent ReadEvent() noexcept;
private:
	std::queue<MouseEvent> eventBuffer;
	bool isLeftDown = false;
	bool isRightDown = false;
	bool isMiddleDown = false;
	bool isInWindow = false;
	int x = 0, y = 0;
};

#endif