#pragma once

#include "glm/glm.hpp"

unsigned int ScaleColor( const unsigned int c, const unsigned int scale );
unsigned int AddBlend( const unsigned int c1, const unsigned int c2 );
unsigned int SubBlend( unsigned int a_Color1, unsigned int a_Color2 );
unsigned int RGBF32_to_RGB8( const glm::fvec4* v );