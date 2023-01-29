#version 330

in vec4 p;
in vec2 t;

out vec2 uv;

void main()
{
	uv=t;
	gl_Position=p;
}