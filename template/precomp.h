// Template, IGAD version 3
// IGAD/NHTV/UU - Jacco Bikker - 2006-2022

// add your includes to this file instead of to individual .cpp files
// to enjoy the benefits of precompiled headers:
// - fast compilation
// - solve issues with the order of header files once (here)
// do not include headers in header files (ever).

// C++ headers


// header for AVX, and every technology before it.
// if your CPU does not support this (unlikely), include the appropriate header instead.
// see: https://stackoverflow.com/a/11228864/2844473
#include <immintrin.h>

// clang-format off


// basic types
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
#ifdef _MSC_VER
typedef unsigned char BYTE;		// for freeimage.h
typedef unsigned short WORD;	// for freeimage.h
typedef unsigned long DWORD;	// for freeimage.h
typedef int BOOL;				// for freeimage.h
#endif

// aligned memory allocations
#ifdef _MSC_VER
#define ALIGN( x ) __declspec( align( x ) )
#define MALLOC64( x ) ( ( x ) == 0 ? 0 : _aligned_malloc( ( x ), 64 ) )
#define FREE64( x ) _aligned_free( x )
#else
#define ALIGN( x ) __attribute__( ( aligned( x ) ) )
#define MALLOC64( x ) ( ( x ) == 0 ? 0 : aligned_alloc( 64, ( x ) ) )
#define FREE64( x ) free( x )
#endif
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RESULT __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RESULT _Check_return_
#else
#define CHECK_RESULT
#endif


// fatal error reporting (with a pretty window)

// OpenGL texture wrapper


// generic error checking for OpenGL code
#define CheckGL() { _CheckGL( __FILE__, __LINE__ ); }

// forward declarations of helper functions

// timer


// swap
template <class T> void Swap( T& x, T& y ) { T t; t = x, x = y, y = t; }

#include "common.h"