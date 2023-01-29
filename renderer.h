#pragma once

#include "app.h"
#include "glm/glm.hpp"
#include "ray.h"
#include "camera.h"
#include "ViewPlane.h"

class Renderer : public App
{
public:
	// game flow methods
	void Init() override;
	glm::fvec3 Trace( Ray& ray );
	void Tick( float deltaTime ) override;
	void Render( class GLTexture *renderTarget ) override;
	void Shutdown()  override { /* implement if you want to do something on exit */ }
	// input handling
	void MouseUp( int button )  override { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button )  override { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y )  override { mousePos.x = x, mousePos.y = y; }
	void MouseWheel( float y )  override { /* implement if you want to handle the mouse wheel */ }
	void KeyUp( int key )  override { /* implement if you want to handle keys */ }
	void KeyDown( int key )  override { /* implement if you want to handle keys */ }
	// data members
	glm::ivec2 mousePos;
	glm::fvec4* accumulator;
	Camera camera;
	ViewPlane *viewPlane;
};