#include "renderer.h"

#include "gltexture.h"
#include "timer.h"
#include "surface.h"
#include "pixel_ops.h"
#include "memory_ops.h"
#include "rand.h"

// -----------------------------------------------------------
// Initialize the renderer
// -----------------------------------------------------------
void Renderer::Init()
{
	// create fp32 rgb pixel buffer to render to
	accumulator = (glm::fvec4*)MALLOC64( SCRWIDTH * SCRHEIGHT * 16 );
	memset( accumulator, 0, SCRWIDTH * SCRHEIGHT * 16 );
	viewPlane = new ViewPlane(SCRWIDTH, SCRHEIGHT);
}

// -----------------------------------------------------------
// Evaluate light transport
// -----------------------------------------------------------
glm::fvec3 Renderer::Trace( Ray& ray )
{
	return glm::fvec3{1.0f, 0.0f, 1.0f};
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Renderer::Tick( float deltaTime )
{
	Timer t;
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		// trace a primary ray for each pixel on the line
		for (int x = 0; x < SCRWIDTH; x++)
		{
			accumulator[x + y * SCRWIDTH].x = RandomFloat();
			accumulator[x + y * SCRWIDTH].y = RandomFloat();
			accumulator[x + y * SCRWIDTH].z = RandomFloat();
			accumulator[x + y * SCRWIDTH].w = 0.0f;
		}
	}
	
	// performance report - running average - ms, MRays/s
	static float avg = 10, alpha = 1;
	avg = (1 - alpha) * avg + alpha * t.elapsed() * 1000;
	if (alpha > 0.05f) alpha *= 0.5f;
	float fps = 1000 / avg, rps = (SCRWIDTH * SCRHEIGHT) * fps;
	printf( "%5.2fms (%.1fps) - %.1fMrays/s\n", avg, fps, rps / 1000000 );
}

void Renderer::Render( GLTexture *renderTarget )
{
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		for(int x = 0; x < SCRWIDTH; x++)
		{
			viewPlane->PutPixel( x, y, &accumulator[x + y * SCRWIDTH] );
		}
	}

	renderTarget->CopyFrom( viewPlane->surface );
}