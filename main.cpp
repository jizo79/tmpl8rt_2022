#pragma comment( linker, "/subsystem:windows /ENTRY:mainCRTStartup" )

#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <thread>
#include <math.h>
#include <algorithm>
#include <assert.h>
#include <io.h>

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
	Shader* shader = new Shader(
		"#version 330\nin vec4 p;\nin vec2 t;out vec2 u;void main(){u=t;gl_Position=p;}",
		"#version 330\nuniform sampler2D c;in vec2 u;out vec4 f;void main(){f=/*sqrt*/(texture(c,u));}", true);
#else
	// fxaa shader
	Shader* shader = new Shader(
		"#version 330\nin vec4 p;\nin vec2 t;out vec2 uv;void main(){uv=t;gl_Position=p;}",
		// FXAA 3.11 Implementation - effendiian
		// https://www.shadertoy.com/view/ttXGzn
		"#version 330\nuniform sampler2D tex;\nin vec2 uv;out vec4 f; \n"							\
		"#define FXAA_LUMINANCE 			1					\n"									\
		"#define FXAA_EDGE_THRESHOLD	  	(1.0/8.0)			\n"									\
		"#define FXAA_EDGE_THRESHOLD_MIN  	(1.0/24.0)			\n"									\
		"#define FXAA_SEARCH_STEPS			32					\n"									\
		"#define FXAA_SEARCH_ACCELERATION 	1					\n"									\
		"#define FXAA_SEARCH_THRESHOLD		(1.0/4.0)			\n"									\
		"#define FXAA_SUBPIX				2 // 1 is crisper	\n"									\
		"#define FXAA_SUBPIX_CAP			(3.0/4.0)			\n"									\
		"#define FXAA_SUBPIX_TRIM			(1.0/4.0)			\n"									\
		"#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))	\n"							\
		"float lum( vec3 color ) {\n #if FXAA_LUMINANCE == 0 \n"									\
		"	return color.x * 0.2126729 + color.y * 0.7151522 + color.z * 0.0721750; \n"				\
		"#else \n return color.g * (0.587 / 0.299) + color.r; \n #endif \n }"						\
		"float vertEdge( float lumaO, float lumaN, float lumaE, float lumaS,"						\
		"	float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )"					\
		"{	float top = (0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE);"						\
		"	float middle = (0.50 * lumaW) + (-1.0 * lumaO) + (0.50 * lumaE);"						\
		"	float bottom = (0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE);"						\
		"	return abs( top ) + abs( middle ) + abs( bottom ); }"									\
		"float horEdge( float lumaO, float lumaN, float lumaE, float lumaS,"						\
		"	float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )"					\
		"{	float top = (0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW);"						\
		"	float middle = (0.50 * lumaN) + (-1.0 * lumaO) + (0.50 * lumaS);"						\
		"	float bottom = (0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE);"						\
		"	return abs( top ) + abs( middle ) + abs( bottom ); }"									\
		"vec3 fxaa( vec2 textureDimensions, vec2 uv )"												\
		"{	vec2 texel = vec2( 1.0, 1.0 ) / textureDimensions;"										\
		"	vec3 rgbN = texture( tex, uv + vec2( 0, -texel.y ) ).rgb,"								\
		"		 rgbW = texture( tex, uv + vec2( -texel.x, 0 ) ).rgb,"								\
		"		 rgbO = texture( tex, uv + vec2( 0, 0 ) ).rgb,"										\
		"		 rgbE = texture( tex, uv + vec2( texel.x, 0 ) ).rgb,"								\
		"		 rgbS = texture( tex, uv + vec2( 0, texel.y ) ).rgb;"								\
		"	float lumaN = lum( rgbN ), lumaW = lum( rgbW );"										\
		"	float lumaO = lum( rgbO ), lumaE = lum( rgbE ), lumaS = lum( rgbS );"					\
		"	float minLuma = min( lumaO, min( min( lumaN, lumaW ), min( lumaS, lumaE ) ) );"			\
		"	float maxLuma = max( lumaO, max( max( lumaN, lumaW ), max( lumaS, lumaE ) ) );"			\
		"	float localContrast = maxLuma - minLuma;"												\
		"	if (localContrast < max( FXAA_EDGE_THRESHOLD_MIN, maxLuma* FXAA_EDGE_THRESHOLD ))"		\
		"		return rgbO;"																		\
		"	vec3 rgbL = rgbN + rgbW + rgbO + rgbE + rgbS;"											\
		"	float lumaL = (lumaN + lumaW + lumaS + lumaE) * 0.25;"									\
		"	float pixelContrast = abs( lumaL - lumaO );"											\
		"	float contrastRatio = pixelContrast / localContrast;"									\
		"	float lowpassBlend = 0;			\n"														\
		"#if FXAA_SUBPIX == 1				\n"														\
		"	lowpassBlend = max( 0.0, contrastRatio - FXAA_SUBPIX_TRIM ) * FXAA_SUBPIX_TRIM_SCALE;"	\
		"	lowpassBlend = min( FXAA_SUBPIX_CAP, lowpassBlend );	\n"								\
		"#elif FXAA_SUBPIX == 2				\n"														\
		"	lowpassBlend = contrastRatio;	\n"														\
		"#endif								\n"														\
		"	vec3 rgbNW = texture( tex, uv + vec2( -texel.x, -texel.y ) ).rgb,"						\
		"		 rgbNE = texture( tex, uv + vec2( texel.x, -texel.y ) ).rgb,"						\
		"		 rgbSW = texture( tex, uv + vec2( -texel.x, texel.y ) ).rgb,"						\
		"		 rgbSE = texture( tex, uv + vec2( texel.x, texel.y ) ).rgb;"						\
		"	rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);"												\
		"	rgbL *= (1.0 / 9.0);"																	\
		"	float lumaNW = lum( rgbNW ), lumaNE = lum( rgbNE );"									\
		"	float lumaSW = lum( rgbSW ), lumaSE = lum( rgbSE );"									\
		"	float edgeVert = vertEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );" \
		"	float edgeHori = horEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );" \
		"	bool isHorizontal = edgeHori >= edgeVert;"												\
		"	float edgeSign = isHorizontal ? -texel.y : -texel.x;"									\
		"	float gradNeg = isHorizontal ? abs( lumaN - lumaO ) : abs( lumaW - lumaO );"			\
		"	float gradPos = isHorizontal ? abs( lumaS - lumaO ) : abs( lumaE - lumaO );"			\
		"	float lumaNeg = isHorizontal ? ((lumaN + lumaO) * 0.5) : ((lumaW + lumaO) * 0.5);"		\
		"	float lumaPos = isHorizontal ? ((lumaS + lumaO) * 0.5) : ((lumaE + lumaO) * 0.5);"		\
		"	bool isNegative = (gradNeg >= gradPos);"												\
		"	float gradientHighest = isNegative ? gradNeg : gradPos;"								\
		"	float lumaHighest = isNegative ? lumaNeg : lumaPos;"									\
		"	if (isNegative) edgeSign *= -1.0;"														\
		"	vec2 pointN = vec2( 0.0, 0.0 );"														\
		"	pointN.x = uv.x + (isHorizontal ? 0.0 : edgeSign * 0.5);"								\
		"	pointN.y = uv.y + (isHorizontal ? edgeSign * 0.5 : 0.0);"								\
		"	gradientHighest *= FXAA_SEARCH_THRESHOLD;"												\
		"	vec2 pointP = pointN;"																	\
		"	vec2 offset = isHorizontal ? vec2( texel.x, 0.0 ) : vec2( 0.0, texel.y );"				\
		"	float lumaNegEnd = lumaNeg, lumaPosEnd = lumaPos;"										\
		"	bool searchNeg = false, searchPos = false;\n"											\
		"#if FXAA_SEARCH_ACCELERATION == 1\n"														\
		"	pointN -= offset, pointP += offset;\n"													\
		"#elif FXAA_SEARCH_ACCELERATION == 2\n"														\
		"	pointN -= offset * 1.5, pointP += offset * 1.5, offset *= 2;\n"							\
		"#elif FXAA_SEARCH_ACCELERATION == 3\n"														\
		"	pointN -= offset * 2, pointP += offset * 2, offset *= 3;\n"								\
		"#elif FXAA_SEARCH_ACCELERATION == 4\n"														\
		"	pointN -= offset * 2.5, pointP += offset * 2.5, offset *= 4;\n"							\
		"#endif\n"																					\
		"	for (int i = 0; i < FXAA_SEARCH_STEPS; i++) {\n"										\
		"	#if FXAA_SEARCH_ACCELERATION == 1\n"													\
		"		if (!searchNeg) lumaNegEnd = lum( texture( tex, pointN ).rgb );"					\
		"		if (!searchPos) lumaPosEnd = lum( texture( tex, pointP ).rgb );\n"					\
		"	#else \n"																				\
		"		if (!searchNeg) lumaNegEnd = lum( textureGrad( tex, pointN, offset, offset ).rgb );"	\
		"		if (!searchPos) lumaPosEnd = lum( textureGrad( tex, pointP, offset, offset ).rgb );\n"	\
		"	#endif \n"																				\
		"		searchNeg = searchNeg || (abs( lumaNegEnd - lumaHighest ) >= gradientHighest);"		\
		"		searchPos = searchPos || (abs( lumaPosEnd - lumaPos ) >= gradPos);"					\
		"		if (searchNeg && searchPos) break;"													\
		"		if (!searchNeg) pointN -= offset;"													\
		"		if (!searchPos) pointP += offset; }"												\
		"	float distanceNeg = isHorizontal ? uv.x - pointN.x : uv.y - pointN.y;"					\
		"	float distancePos = isHorizontal ? pointP.x - uv.x : pointP.y - uv.y;"					\
		"	bool isCloserToNegative = distanceNeg < distancePos;"									\
		"	float lumaEnd = isCloserToNegative ? lumaNegEnd : lumaPosEnd;"							\
		"	if (((lumaO - lumaNeg) < 0.0) == ((lumaEnd - lumaNeg) < 0.0)) edgeSign = 0.0;"			\
		"	float spanLen = distancePos + distanceNeg;"												\
		"	float dist = isCloserToNegative ? distanceNeg : distancePos;"							\
		"	float subOffs = (0.5 + (dist * (-1.0 / spanLen))) * edgeSign;"							\
		"	vec3 rgbOffset = textureLod( tex, vec2( uv.x + (isHorizontal ? 0.0 :"					\
		"		subOffs), uv.y + (isHorizontal ? subOffs : 0.0) ), 0.0 ).rgb;"						\
		"	return mix( rgbOffset, rgbL, lowpassBlend ); }"											\
		"void main(){f=vec4(sqrt(fxaa(vec2(1240,800),uv)),1);}", true);
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
			if (app->screen) renderTarget->CopyFrom(app->screen);
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