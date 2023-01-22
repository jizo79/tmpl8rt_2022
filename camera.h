#pragma once

#include "glm/glm.hpp"
#include "template/scene.h"

// default screen resolution
#define SCRWIDTH	1280
#define SCRHEIGHT	720
// #define FULLSCREEN
// #define DOUBLESIZE

namespace Tmpl8 {

class Camera
{
public:
	Camera()
	{
		// setup a basic view frustum
		camPos = glm::fvec3( 0, 0, -2 );
		topLeft = glm::fvec3( -aspect, 1, 0 );
		topRight = glm::fvec3( aspect, 1, 0 );
		bottomLeft = glm::fvec3( -aspect, -1, 0 );
	}
	Ray GetPrimaryRay( const int x, const int y )
	{
		// calculate pixel position on virtual screen plane
		const float u = (float)x * (1.0f / SCRWIDTH);
		const float v = (float)y * (1.0f / SCRHEIGHT);
		const glm::fvec3 P = topLeft + u * (topRight - topLeft) + v * (bottomLeft - topLeft);
		return Ray( camPos, glm::normalize( P - camPos ) );
	}
	float aspect = (float)SCRWIDTH / (float)SCRHEIGHT;
	glm::fvec3 camPos;
	glm::fvec3 topLeft, topRight, bottomLeft;
};

}