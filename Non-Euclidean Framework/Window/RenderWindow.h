#pragma once
#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

class WindowContainer;
class RenderWindow
{
public:
	enum class CursorType
	{
		NORMAL,
		LINK,
		UPARROW,
		PRECISION,
		MOVE,
		HMOVE,
		VMOVE,
		HELP,
		TEXT,
		WAIT,
		UNAVAILABLE
	};
	bool Initialize(
		WindowContainer* windowContainer,
		HINSTANCE hInstance,
		const std::string& windowTitle,
		const std::string& windowClass,
		int width,
		int height
	);
	bool ProcessMessages() noexcept;
	HWND GetHWND() const noexcept;
	~RenderWindow() noexcept;

	inline HCURSOR GetCursor( CursorType type ) noexcept { return cursors[type]; }

private:
	void RegisterWindowClass() noexcept;
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitle_Wide = L"";
	std::string windowClass = "";
	std::wstring windowClass_Wide = L"";
	int width, height;
	std::unordered_map<CursorType, HCURSOR> cursors;
};

#endif