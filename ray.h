#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
	Ray() = default;
	Ray( glm::fvec3 origin, glm::fvec3 direction, float distance = 1e34f  );
	glm::fvec3 IntersectionPoint() { return O + t * D; }
	// ray data
#ifndef SPEEDTRIX
	glm::fvec3 O, D, rD;
#else
	union { struct { glm::fvec3 O; float d0; }; __m128 O4; };
	union { struct { glm::fvec3 D; float d1; }; __m128 D4; };
	union { struct { glm::fvec3 rD; float d2; }; __m128 rD4; };
#endif
	float t = 1e34f;
	int objIdx = -1;
	bool inside = false; // true when in medium
};