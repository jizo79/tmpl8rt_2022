

#include "surface.h"
#include "ViewPlane.h"

ViewPlane::ViewPlane( int _hres, int _vres, float _pixelSize, float _gamma )
	:hres( _hres ), vres( _vres ), pixelSize( _pixelSize ), gamma( _gamma )
{
	invGamma = 1.0f / gamma;
	surface = new Surface(vres, hres);
}

ViewPlane::~ViewPlane()
{
	delete surface;
}


