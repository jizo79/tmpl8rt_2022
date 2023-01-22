#pragma once

#include "surface.h"

class GLTexture
{
public:
	enum { DEFAULT = 0, FLOAT = 1, INTTARGET = 2 };
	// constructor / destructor
	GLTexture( unsigned int width, unsigned int height, unsigned int type = DEFAULT );
	~GLTexture();
	// methods
	void Bind( const unsigned int slot = 0 );
	void CopyFrom( Surface* src );
	void CopyTo( Surface* dst );
public:
	// public data members
	GLunsigned int ID = 0;
	unsigned int width = 0, height = 0;
};

