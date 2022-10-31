#include "stdafx.h"
#include <imgui/imgui.h>
#include "WindowContainer.h"

WindowContainer::WindowContainer()
{
	static bool rawInputInitialized = false;
	if ( !rawInputInitialized )
	{
		RAWINPUTDEVICE rid = { 0 };
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if ( RegisterRawInputDevices( &rid, 1, sizeof( rid ) ) == FALSE )
		{
			ErrorLogger::Log( GetLastError(), "Failed to register raw input devices!" );
			exit( -1 );
		}

		rawInputInitialized = true;
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WindowContainer::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
        return true;
    const auto& imio = ImGui::GetIO();

    switch( uMsg )
    {
    case WM_ACTIVATE:
		if ( !cursorEnabled )
		{
			if ( wParam & WA_ACTIVE )
				DisableCursor();
			else
				EnableCursor();
		}
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    // Keyboard Events
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if ( imio.WantCaptureKeyboard )
			return 0;
        unsigned char keycode = static_cast<unsigned char>( wParam );
        if ( keyboard.IsKeysAutoRepeat() )
            keyboard.OnKeyPressed( keycode );
        else
        {
            const bool wasPressed = lParam & 0x40000000;
            if ( !wasPressed )
                keyboard.OnKeyPressed( keycode );
        }
        switch ( wParam )
        {
        case VK_ESCAPE:
            DestroyWindow( renderWindow.GetHWND() );
            PostQuitMessage( 0 );
            return 0;
        }
        return 0;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if ( imio.WantCaptureKeyboard )
			return 0;
        unsigned char keycode = static_cast<unsigned char>( wParam );
        keyboard.OnKeyReleased( keycode );
        return 0;
    }
    case WM_CHAR:
    {
        if ( imio.WantCaptureKeyboard )
			return 0;
        unsigned char ch = static_cast<unsigned char>( wParam );
        if ( keyboard.IsCharsAutoRepeat() )
            keyboard.OnChar( ch );
        else
        {
            const bool wasPressed = lParam & 0x40000000;
            if ( !wasPressed )
                keyboard.OnChar( ch );
        }
        return 0;
    }

    // Mouse Events
    case WM_MOUSEMOVE:
    {
        int x = LOWORD( lParam );
		int y = HIWORD( lParam );
		const POINTS pt = MAKEPOINTS( lParam );
		if ( !cursorEnabled )
		{
			if ( !mouse.IsInWindow() )
			{
				SetCapture( renderWindow.GetHWND() );
				mouse.OnMouseEnter( x, y );
				HideCursor();
			}
			return 0;
		}
		if ( imio.WantCaptureMouse )
			return 0;
		if ( pt.x >= 0 && pt.x < windowSize.x && pt.y >= 0 && pt.y < windowSize.y )
		{
			mouse.OnMouseMove( x, y );
			if ( !mouse.IsInWindow() )
			{
				SetCapture( renderWindow.GetHWND() );
				mouse.OnMouseEnter( x, y );
			}
		}
		else
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
				mouse.OnMouseMove( x, y );
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave( x, y );
			}
		}
		return 0;
    }
    case WM_LBUTTONDOWN:
    {
        SetForegroundWindow( renderWindow.GetHWND() );
		SetCursor( renderWindow.GetCursor( RenderWindow::Color::ORANGE ) );
		if ( imio.WantCaptureMouse )
			return 0;

		int x = LOWORD( lParam );
		int y = HIWORD( lParam );
		mouse.OnLeftPressed( x, y );

		if ( !cursorEnabled )
		{
			ConfineCursor();
			HideCursor();
		}
		return 0;
    }
    case WM_LBUTTONUP:
    {
        SetCursor( renderWindow.GetCursor( RenderWindow::Color::BLUE ) );
		if ( imio.WantCaptureMouse )
			return 0;

		int x = LOWORD( lParam );
		int y = HIWORD( lParam );
		mouse.OnLeftReleased( x, y );

		const POINTS pt = MAKEPOINTS( lParam );
		if ( pt.x < 0 || pt.x >= windowSize.x || pt.y < 0 || pt.y >= windowSize.y )
		{
			ReleaseCapture();
			mouse.OnMouseLeave( x, y );
		}
		return 0;
    }
    case WM_RBUTTONDOWN:
    {
        if ( imio.WantCaptureMouse )
			return 0;

        int x = LOWORD( lParam );
        int y = HIWORD( lParam );
        mouse.OnRightPressed( x, y );
        return 0;
    }
    case WM_RBUTTONUP:
    {
        if ( imio.WantCaptureMouse )
			return 0;

		int x = LOWORD( lParam );
		int y = HIWORD( lParam );
		mouse.OnRightReleased( x, y );

		const POINTS pt = MAKEPOINTS( lParam );
		if ( pt.x < 0 || pt.x >= windowSize.x || pt.y < 0 || pt.y >= windowSize.y )
		{
			ReleaseCapture();
			mouse.OnMouseLeave( x, y );
		}
		return 0;
    }
    case WM_MBUTTONDOWN:
    {
        if ( imio.WantCaptureMouse )
			return 0;
        int x = LOWORD( lParam );
        int y = HIWORD( lParam );
        mouse.OnMiddlePressed( x, y );
        return 0;
    }
    case WM_MBUTTONUP:
    {
        if ( imio.WantCaptureMouse )
			return 0;
        int x = LOWORD( lParam );
        int y = HIWORD( lParam );
        mouse.OnMiddleReleased( x, y );
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        if ( imio.WantCaptureMouse )
			return 0;
        int x = LOWORD( lParam );
        int y = HIWORD( lParam );
        if ( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
        {
            mouse.OnWheelUp( x, y );
        }
        else if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
        {
            mouse.OnWheelDown( x, y );
        }
        return 0;
    }
    case WM_INPUT:
    {
        UINT dataSize;
        GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, NULL, &dataSize, sizeof( RAWINPUTHEADER ) );
        if ( dataSize > 0 )
        {
            std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>( dataSize );
            if ( GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, rawData.get(), &dataSize, sizeof( RAWINPUTHEADER ) ) == dataSize )
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>( rawData.get() );
                if ( raw->header.dwType == RIM_TYPEMOUSE )
                {
                    mouse.OnMouseMoveRaw( raw->data.mouse.lLastX, raw->data.mouse.lLastY );
                }
            }
        }
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }
    // Window Resize Event
    case WM_SIZE:
	{
		RECT windowRect = { 0,0 };
		if ( GetClientRect( renderWindow.GetHWND(), &windowRect ) )
		{
			windowSize = {
                static_cast<float>( windowRect.right - windowRect.left ),
                static_cast<float>( windowRect.bottom - windowRect.top )
            };

			if ( windowSize.x < 500 )
				windowSize.x = 1260;

			if ( windowSize.y < 400 )
				windowSize.y = 500;

			return DefWindowProc( hWnd, uMsg, wParam, lParam );;
		}
	}

    default:
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }

    return 0;
}

void WindowContainer::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void WindowContainer::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

void WindowContainer::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect( renderWindow.GetHWND(), &rect );
	MapWindowPoints( renderWindow.GetHWND(), nullptr, reinterpret_cast<POINT*>( &rect ), 2 );
	ClipCursor( &rect );
}

void WindowContainer::FreeCursor() noexcept
{
	ClipCursor( nullptr );
}

void WindowContainer::ShowCursor() noexcept
{
	while ( ::ShowCursor( TRUE ) < 0 );
}

void WindowContainer::HideCursor() noexcept
{
	while ( ::ShowCursor( FALSE ) >= 0 );
}

void WindowContainer::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void WindowContainer::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}