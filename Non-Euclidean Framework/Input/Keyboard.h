#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <queue>

class Keyboard
{
public:
	class KeyboardEvent
	{
	public:
		enum EventType
		{
			Press,
			Release,
			Invalid
		};
		KeyboardEvent();
		KeyboardEvent( const EventType type, const unsigned char key );
		bool IsPress() const noexcept;
		bool IsRelease() const noexcept;
		bool IsValid() const noexcept;
		unsigned char GetKeyCode() const noexcept;
	private:
		EventType type;
		unsigned char key;
	};
public:
	Keyboard();
	bool KeyIsPressed( const unsigned char keycode );
	bool KeyBufferIsEmpty() const noexcept;
	bool CharBufferIsEmpty() const noexcept;
	KeyboardEvent ReadKey() noexcept;
	unsigned char ReadChar() noexcept;
	void OnKeyPressed( const unsigned char key ) noexcept;
	void OnKeyReleased( const unsigned char key ) noexcept;
	void OnChar( const unsigned char key ) noexcept;
	void EnableAutoRepeatKeys() noexcept;
	void DisableAutoRepeatKeys() noexcept;
	void EnableAutoRepeatChars() noexcept;
	void DisableAutoRepeatChars() noexcept;
	bool IsKeysAutoRepeat() const noexcept;
	bool IsCharsAutoRepeat() const noexcept;
private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

#endif