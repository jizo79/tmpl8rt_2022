#include "helpers.h"
#include <fstream>
#include <cstdarg>

void FatalError(const char* fmt, ...)
{
	char t[16384];
	va_list args;
	va_start(args, fmt);
	vsnprintf(t, sizeof(t), fmt, args);
	va_end(args);
// #ifdef _MSC_VER
//	MessageBox(NULL, t, "Fatal error", MB_OK);
// #else
	fprintf(stderr, t);
// #endif
	while (1) exit(0);
}

bool FileIsNewer(const char* file1, const char* file2)
{
	struct stat f1;
	struct stat f2;

	auto ret = stat(file1, &f1);
	FATALERROR_IF(ret, "File %s not found!", file1);

	if (stat(file2, &f2)) return true; // second file does not exist

#ifdef _MSC_VER
	return f1.st_mtime >= f2.st_mtime;
#else
	if (f1.st_mtim.tv_sec >= f2.st_mtim.tv_sec)
		return true;
	return f1.st_mtim.tv_nsec >= f2.st_mtim.tv_nsec;
#endif
}

bool FileExists(const char* f)
{
	std::ifstream s(f);
	return s.good();
}

bool RemoveFile(const char* f)
{
	if (!FileExists(f)) return false;
	return !remove(f);
}

unsigned int FileSize(std::string filename)
{
	std::ifstream s(filename);
	return s.good();
}

std::string TextFileRead(const char* _File)
{
	std::ifstream s(_File);
	std::string str((std::istreambuf_iterator<char>(s)), std::istreambuf_iterator<char>());
	s.close();
	return str;
}

int LineCount(const std::string s)
{
	const char* p = s.c_str();
	int lines = 0;
	while (*p) if (*p++ == '\n') lines++;
	return lines;
}

void TextFileWrite(const std::string& text, const char* _File)
{
	std::ofstream s(_File, std::ios::binary);
	int len = (int)text.size();
	s.write((const char*)&len, sizeof(len));
	s.write(text.c_str(), len);
}
