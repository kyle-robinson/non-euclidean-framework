#pragma once
#ifndef COLOUR_H
#define COLOUR_H

typedef unsigned char BYTE;

class Colour
{
public:
	Colour();
	Colour( unsigned int value );
	Colour( BYTE r, BYTE g, BYTE b );
	Colour( BYTE r, BYTE g, BYTE b, BYTE a );
	Colour( const Colour& src );
public:
	Colour& operator=( const Colour& src );
	bool operator==( const Colour& rhs ) const;
	bool operator!=( const Colour& rhs ) const;
public:
	constexpr BYTE GetR() const;
	void SetR( BYTE r );
	constexpr BYTE GetG() const;
	void SetG( BYTE g );
	constexpr BYTE GetB() const;
	void SetB( BYTE b );
	constexpr BYTE GetA() const;
	void SetA( BYTE a );
private:
	union
	{
		BYTE rgba[4];
		unsigned int colour;
	};
};

namespace Colours
{
	const Colour UnloadedTextureColour( 100, 100, 100 );
	const Colour UnhandledTextureColour( 250, 0, 0 );
}

#endif