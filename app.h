#pragma once

class App
{
public:
	virtual void Init() = 0;
	virtual void Tick( float deltaTime ) = 0;
	virtual void Render( class GLTexture *renderTarget ) = 0;
	virtual void Shutdown() = 0;
	virtual void MouseUp( int button ) = 0;
	virtual void MouseDown( int button ) = 0;
	virtual void MouseMove( int x, int y ) = 0;
	virtual void MouseWheel( float y ) = 0;
	virtual void KeyUp( int key ) = 0;
	virtual void KeyDown( int key ) = 0;
	class Surface* screen = 0;
};