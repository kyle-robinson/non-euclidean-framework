#include "stdafx.h"
#include "StringConverter.h"

std::wstring StringConverter::StringToWide( const std::string& narrow ) noexcept
{
	wchar_t wide[512];
	mbstowcs_s( nullptr, wide, narrow.c_str(), _TRUNCATE );
	return wide;
}

std::string StringConverter::StringToNarrow( const std::wstring& wide ) noexcept
{
	char narrow[512];
	wcstombs_s( nullptr, narrow, wide.c_str(), _TRUNCATE );
	return narrow;
}

std::string StringConverter::GetDirectoryFromPath( const std::string& filePath )
{
	size_t offset1 = filePath.find_last_of( '\\' );
	size_t offset2 = filePath.find_last_of( '/' );
	if ( offset1 == std::string::npos && offset2 == std::string::npos )
		return "";
	if ( offset1 == std::string::npos )
		return filePath.substr( 0, offset2 );
	if ( offset2 == std::string::npos )
		return filePath.substr( 0, offset1 );
	return filePath.substr( 0, std::max( offset1, offset2 ) );
}

std::string StringConverter::GetFileExtension( const std::string& fileName )
{
	size_t offset = fileName.find_last_of('.');
	if ( offset == std::string::npos )
		return {};
	std::string a=std::string( fileName.substr( offset + 1 ) );
	return a;
}