#pragma once
#ifndef WINDOWCONTAINER_H
#define WINDOWCONTAINER_H

#include "Mouse.h"
#include "Keyboard.h"
#include "Graphics.h"
#include "RenderWindow.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	inline bool IsCursorEnabled() const noexcept { return cursorEnabled; }

protected:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;

	bool cursorEnabled = true;
	XMFLOAT2 windowSize;

	RenderWindow renderWindow;
	Graphics graphics;
	Keyboard keyboard;
	Mouse mouse;
};

#endif