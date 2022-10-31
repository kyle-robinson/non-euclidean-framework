#pragma once
#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

class WindowContainer;
class RenderWindow
{
public:
	enum class Color
	{
		BLUE,
		RED,
		GRAY,
		PURPLE,
		ORANGE,
		YELLOW,
		GREEN,
		PINK
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

	inline HCURSOR GetCursor( Color color ) noexcept { return cursors[color]; }

private:
	void RegisterWindowClass() noexcept;
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitle_Wide = L"";
	std::string windowClass = "";
	std::wstring windowClass_Wide = L"";
	int width, height;
	std::unordered_map<Color, HCURSOR> cursors;
};

#endif