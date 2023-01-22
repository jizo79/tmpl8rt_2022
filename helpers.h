#pragma once

#include <string>

void FatalError(const char* fmt, ...);
bool FileIsNewer( const char* file1, const char* file2 );
bool FileExists( const char* f );
bool RemoveFile( const char* f );
std::string TextFileRead( const char* _File );
int LineCount( const std::string s );
void TextFileWrite( const std::string& text, const char* _File );

#define FATALERROR( fmt, ... ) FatalError( "Error on line %d of %s: " fmt "\n", __LINE__, __FILE__, ##__VA_ARGS__ )
#define FATALERROR_IF( condition, fmt, ... ) do { if ( ( condition ) ) FATALERROR( fmt, ##__VA_ARGS__ ); } while ( 0 )
#define FATALERROR_IN( prefix, errstr, fmt, ... ) FatalError( prefix " returned error '%s' at %s:%d" fmt "\n", errstr, __FILE__, __LINE__, ##__VA_ARGS__ );
#define FATALERROR_IN_CALL( stmt, error_parser, fmt, ... ) do { auto ret = ( stmt ); if ( ret ) FATALERROR_IN( #stmt, error_parser( ret ), fmt, ##__VA_ARGS__ ) } while ( 0 )
