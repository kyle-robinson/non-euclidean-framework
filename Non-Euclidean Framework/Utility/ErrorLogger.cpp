#include "stdafx.h"
#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log( const std::string& message ) noexcept
{
	std::string errorMessage = "Error: " + message;
	MessageBoxA( NULL, errorMessage.c_str(), "ERROR", MB_ICONERROR );
}

void ErrorLogger::Log( HRESULT hr, const std::string& message ) noexcept
{
	_com_error error( hr );
	std::wstring errorMessage = L"Error: " + StringConverter::StringToWide( message ) + L"\n" + error.ErrorMessage();
	MessageBoxW( NULL, errorMessage.c_str(), L"ERROR", MB_ICONERROR );
}

void ErrorLogger::Log( HRESULT hr, const std::wstring& message ) noexcept
{
	_com_error error( hr );
	std::wstring errorMessage = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW( NULL, errorMessage.c_str(), L"ERROR", MB_ICONERROR );
}

void ErrorLogger::Log( COMException& exception ) noexcept
{
	std::wstring errorMessage = exception.what();
	MessageBoxW( NULL, errorMessage.c_str(), L"Error: ", MB_ICONERROR );
}