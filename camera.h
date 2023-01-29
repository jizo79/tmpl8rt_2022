#pragma once

#include "glm/glm.hpp"
#include "ray.h"

// default screen resolution
#define SCRWIDTH	1280
#define SCRHEIGHT	720
// #define FULLSCREEN
// #define DOUBLESIZE

class Camera
{
public:
	Camera()
		:aspect((float)SCRWIDTH / (float)SCRHEIGHT),
		camPos( 0, 0, -2 ),
		topLeft( -aspect, 1, 0 ),
		topRight( aspect, 1, 0 ),
		bottomLeft( -aspect, -1, 0 )
	{
	}

	Ray GetPrimaryRay( const int x, const int y ) const
	{
		// calculate pixel position on virtual screen plane
		const float u = (float)x * (1.0f / SCRWIDTH);
		const float v = (float)y * (1.0f / SCRHEIGHT);
		const glm::fvec3 P = topLeft + u * (topRight - topLeft) + v * (bottomLeft - topLeft);
		const glm::fvec3 norm = glm::normalize( P - camPos );
		return Ray{ camPos, norm };
	}

	glm::fvec2 GetScreenUV( const int x, const int y ) const
	{
		const float u = (float)x * (1.0f / SCRWIDTH);
		const float v = (float)y * (1.0f / SCRHEIGHT);
		return glm::fvec2{u, v};
	}

	float aspect;
	glm::fvec3 camPos;
	glm::fvec3 topLeft, topRight, bottomLeft;
};