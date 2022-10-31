#include "stdafx.h"
#include "Mouse.h"

/*   MOUSE EVENT   */
Mouse::MouseEvent::MouseEvent() : type( EventType::Invalid ), x( 0 ), y( 0 )
{ }

Mouse::MouseEvent::MouseEvent( const EventType type, const int x, const int y ) : type( type ), x( x ), y( y )
{ }

bool Mouse::MouseEvent::IsValid() const noexcept
{
	return type != EventType::Invalid;
}

Mouse::MouseEvent::EventType Mouse::MouseEvent::GetType() const noexcept
{
	return type;
}

MousePoint Mouse::MouseEvent::GetPos() const noexcept
{
	return { x, y };
}

int Mouse::MouseEvent::GetPosX() const noexcept
{
	return x;
}

int Mouse::MouseEvent::GetPosY() const noexcept
{
	return y;
}

/*   MOUSE CLASS   */
void Mouse::OnLeftPressed( int x, int y ) noexcept
{
	isLeftDown = true;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::LPress, x, y ) );
}

void Mouse::OnLeftReleased( int x, int y ) noexcept
{
	isLeftDown = false;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::LRelease, x, y ) );
}

void Mouse::OnRightPressed( int x, int y ) noexcept
{
	isRightDown = true;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::RPress, x, y ) );
}

void Mouse::OnRightReleased( int x, int y ) noexcept
{
	isRightDown = false;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::RRelease, x, y ) );
}

void Mouse::OnMiddlePressed( int x, int y ) noexcept
{
	isMiddleDown = true;
	eventBuffer.push( MouseEvent( Mouse::MouseEvent::EventType::MPress, x, y ) );
}

void Mouse::OnMiddleReleased( int x, int y ) noexcept
{
	isMiddleDown = false;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::MRelease, x, y ) );
}

void Mouse::OnWheelUp( int x, int y ) noexcept
{
	eventBuffer.push( MouseEvent( MouseEvent::EventType::WheelUp, x, y ) );
}

void Mouse::OnWheelDown( int x, int y ) noexcept
{
	eventBuffer.push( MouseEvent( MouseEvent::EventType::WheelDown, x, y ) );
}

void Mouse::OnMouseMove( int x, int y ) noexcept
{
	x = x;
	y = y;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::Move, x, y ) );
}

void Mouse::OnMouseMoveRaw( int x, int y ) noexcept
{
	eventBuffer.push( MouseEvent( MouseEvent::EventType::RawMove, x, y ) );
}

void Mouse::OnMouseEnter( int x, int y ) noexcept
{
	isInWindow = true;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::Enter, x, y ) );
}

void Mouse::OnMouseLeave( int x, int y ) noexcept
{
	isInWindow = false;
	eventBuffer.push( MouseEvent( MouseEvent::EventType::Leave, x, y ) );
}

bool Mouse::IsLeftDown() const noexcept
{
	return isLeftDown;
}

bool Mouse::IsRightDown() const noexcept
{
	return isRightDown;
}

bool Mouse::IsMiddleDown() const noexcept
{
	return isMiddleDown;
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

int Mouse::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetPosY() const noexcept
{
	return y;
}

MousePoint Mouse::GetPos() const noexcept
{
	return { x, y };
}

bool Mouse::EventBufferIsEmpty() const noexcept
{
	return eventBuffer.empty();
}

Mouse::MouseEvent Mouse::ReadEvent() noexcept
{
	if ( eventBuffer.empty() )
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = eventBuffer.front();
		eventBuffer.pop();
		return e;
	}
}