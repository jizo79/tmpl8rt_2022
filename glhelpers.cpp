#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#endif
#include "windows.h"

#include "glhelpers.h"
#include "helpers.h"

void _CheckGL(const char* f, int l)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const char* errStr = "UNKNOWN ERROR";
		if (error == 0x500) errStr = "INVALID ENUM";
		else if (error == 0x502) errStr = "INVALID OPERATION";
		else if (error == 0x501) errStr = "INVALID VALUE";
		else if (error == 0x506) errStr = "INVALID FRAMEBUFFER OPERATION";
		FatalError("GL error %d: %s at %s:%d\n", error, errStr, f, l);
	}
}

GLuint CreateVBO(const GLfloat* data, const unsigned int size)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	CheckGL();
	return id;
}

void BindVBO(const unsigned int idx, const unsigned int N, const GLuint id)
{
	glEnableVertexAttribArray(idx);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glVertexAttribPointer(idx, N, GL_FLOAT, GL_FALSE, 0, (void*)0);
	CheckGL();
}

void CheckShader(GLuint shader, const char* vshader, const char* fshader)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	GLsizei length = 0;
	glGetShaderInfoLog(shader, sizeof(buffer), &length, buffer);
	CheckGL();
	FATALERROR_IF(length > 0 && strstr(buffer, "ERROR"), "Shader compile error:\n%s", buffer);
}

void CheckProgram(GLuint id, const char* vshader, const char* fshader)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	GLsizei length = 0;
	glGetProgramInfoLog(id, sizeof(buffer), &length, buffer);
	CheckGL();
	FATALERROR_IF(length > 0, "Shader link error:\n%s", buffer);
}

void DrawQuad()
{
	static GLuint vao = 0;
	if (!vao)
	{
		// generate buffers
		static const GLfloat verts[] = { -1, 1, 0, 1, 1, 0, -1, -1, 0, 1, 1, 0, -1, -1, 0, 1, -1, 0 };
		static const GLfloat uvdata[] = { 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1 };
		GLuint vertexBuffer = CreateVBO(verts, sizeof(verts));
		GLuint UVBuffer = CreateVBO(uvdata, sizeof(uvdata));
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		BindVBO(0, 3, vertexBuffer);
		BindVBO(1, 2, UVBuffer);
		glBindVertexArray(0);
		CheckGL();
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}