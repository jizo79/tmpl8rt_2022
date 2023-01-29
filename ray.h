#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
	Ray() = default;
	Ray( glm::fvec3 origin, glm::fvec3 direction, float distance = 1e34f  );
	glm::fvec3 IntersectionPoint() { return O + t * D; }

	// ray data
	glm::fvec3 O, D, rD;

	float t = 1e34f;
	int objIdx = -1;
	bool inside = false; // true when in medium
};