#pragma once

inline unsigned int ScaleColor( const unsigned int c, const unsigned int scale );
inline unsigned int AddBlend( const unsigned int c1, const unsigned int c2 );
inline unsigned int SubBlend( unsigned int a_Color1, unsigned int a_Color2 );
inline unsigned int RGBF32_to_RGB8( const float4* v );