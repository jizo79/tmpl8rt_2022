#pragma once

#include "glm/glm.hpp"

inline unsigned int RGBF32_to_RGB8( const glm::fvec4* v )
{
	unsigned int r = (unsigned int)(255.0f * glm::min( 1.0f, v->x ));
	unsigned int g = (unsigned int)(255.0f * glm::min( 1.0f, v->y ));
	unsigned int b = (unsigned int)(255.0f * glm::min( 1.0f, v->z ));
	return (r << 16) + (g << 8) + b;
}