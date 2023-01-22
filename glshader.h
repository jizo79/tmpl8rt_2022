#pragma once

#include "glm/glm.hpp"

class Shader
{
public:
	// constructor / destructor
	Shader( const char* vfile, const char* pfile, bool fromString );
	~Shader();
	// methods
	void Init( const char* vfile, const char* pfile );
	void Compile( const char* vtext, const char* ftext );
	void Bind();
	void SetInputTexture( unsigned int slot, const char* name, class GLTexture* texture );
	void SetInputMatrix( const char* name, const glm::fmat4x4& matrix );
	void SetFloat( const char* name, const float v );
	void SetInt( const char* name, const int v );
	void SetUInt( const char* name, const unsigned int v );
	void Unbind();
private:
	// data members
	unsigned int vertex = 0;	// vertex shader identifier
	unsigned int pixel = 0;		// fragment shader identifier
	unsigned int ID = 0;		// shader program identifier
};
