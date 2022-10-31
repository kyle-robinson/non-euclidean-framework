#pragma once
#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include "COMException.h"

/// <summary>
/// Provides a list of static functions to log errors in a MessageBox.
/// MessageBox errors use macros to output the file, function and line that an error was thrown.
/// </summary>
class ErrorLogger
{
public:
	static void Log( const std::string& message ) noexcept;
	static void Log( HRESULT hr, const std::string& message ) noexcept;
	static void Log( HRESULT hr, const std::wstring& message ) noexcept;
	static void Log( COMException& exception ) noexcept;
};

#endif