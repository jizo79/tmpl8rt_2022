#pragma once

class Surface
{
	enum { OWNER = 1 };
public:
	// constructor / destructor
	Surface() = default;
	Surface( int w, int h, unsigned int* buffer );
	Surface( int w, int h );
	Surface( const char* file );
	~Surface();

	// operations
	void Clear( unsigned int c );
	void LoadImage( const char* file );
	void CopyTo( Surface* dst, int x, int y );

	// attributes
	unsigned int* pixels = 0;
	int width = 0, height = 0;
	bool ownBuffer = false;
};
