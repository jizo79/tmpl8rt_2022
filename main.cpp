#pragma comment( linker, "/subsystem:windows /ENTRY:mainCRTStartup" )

#include <fstream>
#include <vector>
#include <thread>
#include <assert.h>

#define GLFW_USE_CHDIR 0
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#endif
#include "windows.h"

#include "gltexture.h"
#include "glhelpers.h"
#include "renderer.h"
#include "helpers.h"
#include "glshader.h"
#include "timer.h"

static GLFWwindow* window = 0;
static bool hasFocus = true, running = true;
static GLTexture* renderTarget = 0;
static int scrwidth = 0, scrheight = 0;
static App* app = 0;

GLTexture* GetRenderTarget()
{
	return renderTarget;
}



void ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}

void InitRenderTarget(int w, int h)
{
	// allocate render target and surface
	scrwidth = w, scrheight = h;
	renderTarget = new GLTexture(scrwidth, scrheight, GLTexture::INTTARGET);
}
void ReshapeWindowCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}
void KeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE) running = false;
	if (action == GLFW_PRESS) { if (app) if (key >= 0) app->KeyDown(key); }
	else if (action == GLFW_RELEASE) { if (app) if (key >= 0) app->KeyUp(key); }
}
void CharEventCallback(GLFWwindow* window, unsigned int code) { /* nothing here yet */ }
void WindowFocusCallback(GLFWwindow* window, int focused) { hasFocus = (focused == GL_TRUE); }
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) { if (app) app->MouseDown(button); }
	else if (action == GLFW_RELEASE) { if (app) app->MouseUp(button); }
}
void MouseScrollCallback(GLFWwindow* window, double x, double y)
{
	app->MouseWheel((float)y);
}
void MousePosCallback(GLFWwindow* window, double x, double y)
{
	if (app) app->MouseMove((int)x, (int)y);
}

void main()
{

	// open a window
	if (!glfwInit()) FatalError("glfwInit failed.");
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3.3 is enough for our needs
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_STENCIL_BITS, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE /* easier :) */);
#ifdef FULLSCREEN
	window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "Tmpl8RT", glfwGetPrimaryMonitor(), 0);
#else
#ifdef DOUBLESIZE
	window = glfwCreateWindow(SCRWIDTH * 2, SCRHEIGHT * 2, "Tmpl8RT", 0, 0);
#else
	window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "Tmpl8RT", 0, 0);
#endif
#endif
	if (!window) FatalError("glfwCreateWindow failed.");
	glfwMakeContextCurrent(window);
	// register callbacks
	glfwSetWindowSizeCallback(window, ReshapeWindowCallback);
	glfwSetKeyCallback(window, KeyEventCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetCursorPosCallback(window, MousePosCallback);
	glfwSetCharCallback(window, CharEventCallback);
	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) FatalError("gladLoadGLLoader failed.");
	glfwSwapInterval(0);
	// prepare OpenGL state
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	CheckGL();
	// we want a console window for text output
#ifndef FULLSCREEN
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.X = 1280;
	coninfo.dwSize.Y = 800;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	FILE* file = nullptr;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetWindowPos(GetConsoleWindow(), HWND_TOP, 0, 0, 1280, 800, 0);
	glfwShowWindow(window);
#endif
	// initialize application
	InitRenderTarget(SCRWIDTH, SCRHEIGHT);
	Surface* screen = new Surface(SCRWIDTH, SCRHEIGHT);
	app = new Renderer();
	app->screen = screen;
	app->Init();
	// done, enter main loop
#if 1
	// basic shader: apply gamma correction
	Shader* shader = new Shader("basic.vert",  "basic.frag", false);
#else
	// fxaa shader
	Shader* shader = new Shader("fxaa.vert", "fxaa.frag", false);
		
#endif
	float deltaTime = 0;
	static int frameNr = 0;
	static Timer timer;
	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glm::min(500.0f, 1000.0f * timer.elapsed());
		timer.reset();
		app->Tick(deltaTime);
		// send the rendering result to the screen using OpenGL
		if (frameNr++ > 1)
		{
			app->Render( renderTarget );
			shader->Bind();
			shader->SetInputTexture(0, "c", renderTarget);
			DrawQuad();
			shader->Unbind();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		if (!running) break;
	}
	// close down
	app->Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
}