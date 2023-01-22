#pragma once

unsigned int InitSeed( unsigned int seedBase );
unsigned int RandomUInt();
unsigned int RandomUInt( unsigned int& seed );
float RandomFloat();
float RandomFloat( unsigned int& seed );
float Rand( float range );

// Perlin noise
float noise2D( const float x, const float y );
