// FXAA 3.11 Implementation - effendiian
// https://www.shadertoy.com/view/ttXGzn

#version 330

uniform sampler2D tex;

in vec2 uv;

out vec4 f;

#define FXAA_LUMINANCE 				1
#define FXAA_EDGE_THRESHOLD	  		(1.0/8.0)
#define FXAA_EDGE_THRESHOLD_MIN  	(1.0/24.0)
#define FXAA_SEARCH_STEPS			32
#define FXAA_SEARCH_ACCELERATION 	1
#define FXAA_SEARCH_THRESHOLD		(1.0/4.0)
#define FXAA_SUBPIX					2 // 1 is crisper
#define FXAA_SUBPIX_CAP				(3.0/4.0)
#define FXAA_SUBPIX_TRIM			(1.0/4.0)
#define FXAA_SUBPIX_TRIM_SCALE		(1.0/(1.0 - FXAA_SUBPIX_TRIM))

float lum( vec3 color )
{
#if FXAA_LUMINANCE == 0
	return color.x * 0.2126729 + color.y * 0.7151522 + color.z * 0.0721750;
#else
	return color.g * (0.587 / 0.299) + color.r;
#endif
}

float vertEdge( float lumaO, float lumaN, float lumaE, float lumaS, float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )
{
	float top = (0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE);
	float middle = (0.50 * lumaW) + (-1.0 * lumaO) + (0.50 * lumaE);
	float bottom = (0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE);
	return abs( top ) + abs( middle ) + abs( bottom );
}

float horEdge( float lumaO, float lumaN, float lumaE, float lumaS, float lumaW, float lumaNW, float lumaNE, float lumaSW, float lumaSE )
{
	float top = (0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW);
	float middle = (0.50 * lumaN) + (-1.0 * lumaO) + (0.50 * lumaS);
	float bottom = (0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE);
	return abs( top ) + abs( middle ) + abs( bottom );
}

vec3 fxaa( vec2 textureDimensions, vec2 uv )
{
	vec2 texel = vec2( 1.0, 1.0 ) / textureDimensions;
	vec3 rgbN = texture( tex, uv + vec2( 0, -texel.y ) ).rgb;
	vec3 rgbW = texture( tex, uv + vec2( -texel.x, 0 ) ).rgb;
	vec3 rgbO = texture( tex, uv + vec2( 0, 0 ) ).rgb;
	vec3 rgbE = texture( tex, uv + vec2( texel.x, 0 ) ).rgb;
	vec3 rgbS = texture( tex, uv + vec2( 0, texel.y ) ).rgb;

	float lumaN = lum( rgbN );
	float lumaW = lum( rgbW );
	float lumaO = lum( rgbO );
	float lumaE = lum( rgbE );
	float lumaS = lum( rgbS );
	
	float minLuma = min( lumaO, min( min( lumaN, lumaW ), min( lumaS, lumaE ) ) );
	float maxLuma = max( lumaO, max( max( lumaN, lumaW ), max( lumaS, lumaE ) ) );
	float localContrast = maxLuma - minLuma;
	
	if ( localContrast < max( FXAA_EDGE_THRESHOLD_MIN, maxLuma* FXAA_EDGE_THRESHOLD ) )
	{
		return rgbO;
	}
	
	vec3 rgbL = rgbN + rgbW + rgbO + rgbE + rgbS;
	float lumaL = (lumaN + lumaW + lumaS + lumaE) * 0.25;
	float pixelContrast = abs( lumaL - lumaO );
	float contrastRatio = pixelContrast / localContrast;
	float lowpassBlend = 0;

#if FXAA_SUBPIX == 1
	lowpassBlend = max( 0.0, contrastRatio - FXAA_SUBPIX_TRIM ) * FXAA_SUBPIX_TRIM_SCALE;
	lowpassBlend = min( FXAA_SUBPIX_CAP, lowpassBlend );
#elif FXAA_SUBPIX == 2
	lowpassBlend = contrastRatio;
#endif

	vec3 rgbNW = texture( tex, uv + vec2( -texel.x, -texel.y ) ).rgb;
	vec3 rgbNE = texture( tex, uv + vec2( texel.x, -texel.y ) ).rgb;
	vec3 rgbSW = texture( tex, uv + vec2( -texel.x, texel.y ) ).rgb;
	vec3 rgbSE = texture( tex, uv + vec2( texel.x, texel.y ) ).rgb;
	
	rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
	rgbL *= (1.0 / 9.0);
	
	float lumaNW = lum( rgbNW );
	float lumaNE = lum( rgbNE );
	float lumaSW = lum( rgbSW );
	float lumaSE = lum( rgbSE );
	float edgeVert = vertEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );
	float edgeHori = horEdge( lumaO, lumaN, lumaE, lumaS, lumaW, lumaNW, lumaNE, lumaSW, lumaSE );
	bool isHorizontal = edgeHori >= edgeVert;
	float edgeSign = isHorizontal ? -texel.y : -texel.x;
	float gradNeg = isHorizontal ? abs( lumaN - lumaO ) : abs( lumaW - lumaO );
	float gradPos = isHorizontal ? abs( lumaS - lumaO ) : abs( lumaE - lumaO );
	float lumaNeg = isHorizontal ? ((lumaN + lumaO) * 0.5) : ((lumaW + lumaO) * 0.5);
	float lumaPos = isHorizontal ? ((lumaS + lumaO) * 0.5) : ((lumaE + lumaO) * 0.5);
	bool isNegative = (gradNeg >= gradPos);
	float gradientHighest = isNegative ? gradNeg : gradPos;
	float lumaHighest = isNegative ? lumaNeg : lumaPos;
	
	if ( isNegative )
	{
		edgeSign *= -1.0;
	}
	
	vec2 pointN = vec2( 0.0, 0.0 );
	pointN.x = uv.x + (isHorizontal ? 0.0 : edgeSign * 0.5);
	pointN.y = uv.y + (isHorizontal ? edgeSign * 0.5 : 0.0);
	gradientHighest *= FXAA_SEARCH_THRESHOLD;
	
	vec2 pointP = pointN;
	vec2 offset = isHorizontal ? vec2( texel.x, 0.0 ) : vec2( 0.0, texel.y );
	float lumaNegEnd = lumaNeg;
	float lumaPosEnd = lumaPos;
	
	bool searchNeg = false;
	bool searchPos = false;
		
#if FXAA_SEARCH_ACCELERATION == 1
	pointN -= offset, pointP += offset;
#elif FXAA_SEARCH_ACCELERATION == 2
	pointN -= offset * 1.5;
	pointP += offset * 1.5;
	offset *= 2;
#elif FXAA_SEARCH_ACCELERATION == 3
	pointN -= offset * 2;
	pointP += offset * 2;
	offset *= 3;
#elif FXAA_SEARCH_ACCELERATION == 4
	pointN -= offset * 2.5;
	pointP += offset * 2.5; offset *= 4;
#endif

	for (int i = 0; i < FXAA_SEARCH_STEPS; i++) 
	{
	#if FXAA_SEARCH_ACCELERATION == 1
		if (!searchNeg)
		{
			lumaNegEnd = lum( texture( tex, pointN ).rgb );
		}
		if (!searchPos)
		{
			lumaPosEnd = lum( texture( tex, pointP ).rgb );
		}
	#else
		if (!searchNeg)
		{
			lumaNegEnd = lum( textureGrad( tex, pointN, offset, offset ).rgb );
		}
		if (!searchPos)
		{
			lumaPosEnd = lum( textureGrad( tex, pointP, offset, offset ).rgb );
		}
	#endif
	
		searchNeg = searchNeg || (abs( lumaNegEnd - lumaHighest ) >= gradientHighest);
		searchPos = searchPos || (abs( lumaPosEnd - lumaPos ) >= gradPos);
		
		if (searchNeg && searchPos) 
		{
			break;
		}
		
		if (!searchNeg)
		{
			pointN -= offset;
		}

		if (!searchPos)
		{
			pointP += offset; 
		}
	}
	
	float distanceNeg = isHorizontal ? uv.x - pointN.x : uv.y - pointN.y;
	float distancePos = isHorizontal ? pointP.x - uv.x : pointP.y - uv.y;
	bool isCloserToNegative = distanceNeg < distancePos;
	float lumaEnd = isCloserToNegative ? lumaNegEnd : lumaPosEnd;
	
	if ( ( ( lumaO - lumaNeg ) < 0.0 ) == ( ( lumaEnd - lumaNeg ) < 0.0 ) )
	{
		edgeSign = 0.0;
	}
	
	float spanLen = distancePos + distanceNeg;
	float dist = isCloserToNegative ? distanceNeg : distancePos;
	float subOffs = (0.5 + (dist * (-1.0 / spanLen))) * edgeSign;
	vec3 rgbOffset = textureLod( tex, vec2( uv.x + (isHorizontal ? 0.0 : subOffs), uv.y + (isHorizontal ? subOffs : 0.0) ), 0.0 ).rgb;
	
	return mix( rgbOffset, rgbL, lowpassBlend ); 
}

void main()
{
	f=vec4( sqrt( fxaa( vec2( 1240,800 ),uv ) ), 1 );
}