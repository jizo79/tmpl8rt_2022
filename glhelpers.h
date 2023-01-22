#pragma once

#include "glad/glad.h"

void _CheckGL( const char* f, int l );
GLuint CreateVBO( const GLfloat* data, const unsigned int size );
void BindVBO( const unsigned int idx, const unsigned int N, const GLuint id );
void CheckShader( GLuint shader, const char* vshader, const char* fshader );
void CheckProgram( GLuint id, const char* vshader, const char* fshader );
void DrawQuad();

#define CheckGL() { _CheckGL( __FILE__, __LINE__ ); }