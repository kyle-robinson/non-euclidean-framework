#include "stdafx.h"
#include "Keyboard.h"

/*   KEYBOARD EVENT   */
Keyboard::KeyboardEvent::KeyboardEvent() : type( KeyboardEvent::EventType::Invalid ), key( 0u )
{ }

Keyboard::KeyboardEvent::KeyboardEvent( const EventType type, const unsigned char key ) : type( type ), key( key )
{ }

bool Keyboard::KeyboardEvent::IsPress() const noexcept
{
	return type == EventType::Press;
}

bool Keyboard::KeyboardEvent::IsRelease() const noexcept
{
	return type == EventType::Release;
}

bool Keyboard::KeyboardEvent::IsValid() const noexcept
{
	return type != EventType::Invalid;
}

unsigned char Keyboard::KeyboardEvent::GetKeyCode() const noexcept
{
	return key;
}

/*   KEYBOARD CLASS   */
Keyboard::Keyboard()
{
	for ( int i = 0; i < 256; i++ )
		keyStates[i] = false;
}

bool Keyboard::KeyIsPressed( const unsigned char keycode )
{
	return keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty() const noexcept
{
	return keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty() const noexcept
{
	return charBuffer.empty();
}

Keyboard::KeyboardEvent Keyboard::ReadKey() noexcept
{
	if ( keyBuffer.empty() )
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent e = keyBuffer.front(); // get first event from queue
		keyBuffer.pop();
		return e;
	}
}

unsigned char Keyboard::ReadChar() noexcept
{
	if ( charBuffer.empty() )
	{
		return 0u;
	}
	else
	{
		unsigned char e = charBuffer.front();
		charBuffer.pop();
		return e;
	}
}

void Keyboard::OnKeyPressed( const unsigned char key ) noexcept
{
	keyStates[key] = true;
	keyBuffer.push( KeyboardEvent( KeyboardEvent::EventType::Press, key ) );
}

void Keyboard::OnKeyReleased( const unsigned char key ) noexcept
{
	keyStates[key] = false;
	keyBuffer.push( KeyboardEvent( KeyboardEvent::EventType::Release, key ) );
}

void Keyboard::OnChar( const unsigned char key ) noexcept
{
	charBuffer.push( key );
}

void Keyboard::EnableAutoRepeatKeys() noexcept
{
	autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys() noexcept
{
	autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars() noexcept
{
	autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars() noexcept
{
	autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat() const noexcept
{
	return autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat() const noexcept
{
	return autoRepeatChars;
}