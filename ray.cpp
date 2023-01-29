#include "ray.h"

Ray::Ray( glm::fvec3 origin, glm::fvec3 direction, float distance )
	{
		O = origin, D = direction, t = distance;
		rD = glm::fvec3( 1 / D.x, 1 / D.y, 1 / D.z );
	}