#pragma once

#include "glm/glm.hpp"
#include "surface.h"
#include "pixel_ops.h"
class ViewPlane
{
public:
	ViewPlane() = delete;
	ViewPlane( int _hres, int _vres, float _pixelSize = 1.0f, float _gamma = 2.2f);
	~ViewPlane();

	inline void ViewPlane::PutPixel( int &x, int &y, const glm::fvec4 *color )
	{
		surface->pixels[x + y * hres] = RGBF32_to_RGB8( color );
	}

	int hres;
	int vres;
	float pixelSize;
	float gamma;
	float invGamma;

	Surface * surface;
};