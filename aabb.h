#pragma once

#include <xmmintrin.h>
#include "glm/glm.hpp"

class aabb
{
public:
	aabb() = default;
	aabb( __m128 a, __m128 b ) { bmin4 = a, bmax4 = b; bmin[3] = bmax[3] = 0; }
	aabb( glm::fvec3 a, glm::fvec3 b ) { bmin[0] = a.x, bmin[1] = a.y, bmin[2] = a.z, bmin[3] = 0, bmax[0] = b.x, bmax[1] = b.y, bmax[2] = b.z, bmax[3] = 0; }
	__inline void Reset() { bmin4 = _mm_set_ps1( 1e34f ), bmax4 = _mm_set_ps1( -1e34f ); }
	bool Contains( const __m128& p ) const
	{
		union { __m128 va4; float va[4]; };
		union { __m128 vb4; float vb[4]; };
		va4 = _mm_sub_ps( p, bmin4 ), vb4 = _mm_sub_ps( bmax4, p );
		return ((va[0] >= 0) && (va[1] >= 0) && (va[2] >= 0) &&
			(vb[0] >= 0) && (vb[1] >= 0) && (vb[2] >= 0));
	}
	__inline void Grow( const aabb& bb ) { bmin4 = _mm_min_ps( bmin4, bb.bmin4 ); bmax4 = _mm_max_ps( bmax4, bb.bmax4 ); }
	__inline void Grow( const __m128& p ) { bmin4 = _mm_min_ps( bmin4, p ); bmax4 = _mm_max_ps( bmax4, p ); }
	__inline void Grow( const __m128 min4, const __m128 max4 ) { bmin4 = _mm_min_ps( bmin4, min4 ); bmax4 = _mm_max_ps( bmax4, max4 ); }
	__inline void Grow( const glm::fvec3& p ) { __m128 p4 = _mm_setr_ps( p.x, p.y, p.z, 0 ); Grow( p4 ); }
	aabb Union( const aabb& bb ) const { aabb r; r.bmin4 = _mm_min_ps( bmin4, bb.bmin4 ), r.bmax4 = _mm_max_ps( bmax4, bb.bmax4 ); return r; }
	static aabb Union( const aabb& a, const aabb& b ) { aabb r; r.bmin4 = _mm_min_ps( a.bmin4, b.bmin4 ), r.bmax4 = _mm_max_ps( a.bmax4, b.bmax4 ); return r; }
	aabb Intersection( const aabb& bb ) const { aabb r; r.bmin4 = _mm_max_ps( bmin4, bb.bmin4 ), r.bmax4 = _mm_min_ps( bmax4, bb.bmax4 ); return r; }
	__inline float Extend( const int axis ) const { return bmax[axis] - bmin[axis]; }
	__inline float Minimum( const int axis ) const { return bmin[axis]; }
	__inline float Maximum( const int axis ) const { return bmax[axis]; }
	float Area() const
	{
		union { __m128 e4; float e[4]; };
		e4 = _mm_sub_ps( bmax4, bmin4 );
		return glm::max( 0.0f, e[0] * e[1] + e[0] * e[2] + e[1] * e[2] );
	}
	int LongestAxis() const
	{
		int a = 0;
		if (Extend( 1 ) > Extend( 0 )) a = 1;
		if (Extend( 2 ) > Extend( a )) a = 2;
		return a;
	}
	// data members
	union
	{
		struct
		{
			union { __m128 bmin4; float bmin[4]; struct { glm::fvec3 bmin3; }; };
			union { __m128 bmax4; float bmax[4]; struct { glm::fvec3 bmax3; }; };
		};
		__m128 bounds[2] = { _mm_set_ps( 1e34f, 1e34f, 1e34f, 0 ), _mm_set_ps( -1e34f, -1e34f, -1e34f, 0 ) };
	};
	__inline void SetBounds( const __m128 min4, const __m128 max4 ) { bmin4 = min4; bmax4 = max4; }
	__inline __m128 Center() const { return _mm_mul_ps( _mm_add_ps( bmin4, bmax4 ), _mm_set_ps1( 0.5f ) ); }
	__inline float Center( unsigned int axis ) const { return (bmin[axis] + bmax[axis]) * 0.5f; }
};

