#include "pixel_ops.h"
#include "glm/glm.hpp"
#include "glm/ext/vector_float4.hpp"

unsigned int ScaleColor( const unsigned int c, const unsigned int scale )
{
	const unsigned int rb = (((c & 0xff00ff) * scale) >> 8) & 0x00ff00ff;
	const unsigned int ag = (((c & 0xff00ff00) >> 8) * scale) & 0xff00ff00;
	return rb + ag;
}
unsigned int AddBlend( const unsigned int c1, const unsigned int c2 )
{
	const unsigned int r1 = (c1 >> 16) & 255, r2 = (c2 >> 16) & 255;
	const unsigned int g1 = (c1 >> 8) & 255, g2 = (c2 >> 8) & 255;
	const unsigned int b1 = c1 & 255, b2 = c2 & 255;
	const unsigned int r = glm::min( 255u, r1 + r2 );
	const unsigned int g = glm::min( 255u, g1 + g2 );
	const unsigned int b = glm::min( 255u, b1 + b2 );
	return (r << 16) + (g << 8) + b;
}
unsigned int SubBlend( unsigned int a_Color1, unsigned int a_Color2 )
{
	int red = (a_Color1 & 0xff0000) - (a_Color2 & 0xff0000);
	int green = (a_Color1 & 0x00ff00) - (a_Color2 & 0x00ff00);
	int blue = (a_Color1 & 0x0000ff) - (a_Color2 & 0x0000ff);
	if (red < 0) red = 0;
	if (green < 0) green = 0;
	if (blue < 0) blue = 0;
	return (unsigned int)(red + green + blue);
}
unsigned int RGBF32_to_RGB8( const glm::fvec4* v )
{
#ifdef _MSC_VER_
	// based on https://stackoverflow.com/q/29856006
	static __m128 s4 = _mm_set1_ps( 255.0f );
	__m128 a = _mm_load_ps( (const float*)v );
	a = _mm_shuffle_ps( a, a, _MM_SHUFFLE(3, 0, 1, 2) );
	__m128i b = _mm_cvtps_epi32( _mm_mul_ps( a, s4 ) );
	b = _mm_packus_epi32( b, b );
	return _mm_cvtsi128_si32( _mm_packus_epi16( b, b ) );
#else
	unsigned int r = (unsigned int)(255.0f * glm::min( 1.0f, v->x ));
	unsigned int g = (unsigned int)(255.0f * glm::min( 1.0f, v->y ));
	unsigned int b = (unsigned int)(255.0f * glm::min( 1.0f, v->z ));
	return (r << 16) + (g << 8) + b;
#endif
}