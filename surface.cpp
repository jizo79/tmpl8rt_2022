#include "surface.h"
#include "memory_ops.h"
#include "helpers.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "../lib/stb_image.h"

Surface::Surface(int w, int h, unsigned int* b) : pixels(b), width(w), height(h) {}
Surface::Surface(int w, int h) : width(w), height(h)
{
	pixels = (unsigned int*)MALLOC64(w * h * sizeof(unsigned int));
	ownBuffer = true; // needs to be deleted in destructor
}
Surface::Surface(const char* file) : pixels(0), width(0), height(0)
{
	FILE* f = fopen(file, "rb");
	if (!f) FatalError("File not found: %s", file);
	fclose(f);
	LoadImage(file);
}

void Surface::LoadImage(const char* file)
{
	int n;
	unsigned char* data = stbi_load(file, &width, &height, &n, 0);
	if (data)
	{
		pixels = (unsigned int*)MALLOC64(width * height * sizeof(unsigned int));
		ownBuffer = true; // needs to be deleted in destructor
		const int s = width * height;
		if (n == 1) // greyscale
		{
			for (int i = 0; i < s; i++)
			{
				const unsigned char p = data[i];
				pixels[i] = p + (p << 8) + (p << 16);
			}
		}
		else
		{
			for (int i = 0; i < s; i++) pixels[i] = (data[i * n + 0] << 16) + (data[i * n + 1] << 8) + data[i * n + 2];
		}
	}
	stbi_image_free(data);
}

Surface::~Surface()
{
	if (ownBuffer) FREE64(pixels); // free only if we allocated the buffer ourselves
}

void Surface::Clear(unsigned int c)
{
	const int s = width * height;
	for (int i = 0; i < s; i++) pixels[i] = c;
}

void Surface::CopyTo(Surface* d, int x, int y)
{
	unsigned int* dst = d->pixels;
	unsigned int* src = pixels;
	if ((src) && (dst))
	{
		int srcwidth = width;
		int srcheight = height;
		int dstwidth = d->width;
		int dstheight = d->height;
		if ((srcwidth + x) > dstwidth) srcwidth = dstwidth - x;
		if ((srcheight + y) > dstheight) srcheight = dstheight - y;
		if (x < 0) src -= x, srcwidth += x, x = 0;
		if (y < 0) src -= y * srcwidth, srcheight += y, y = 0;
		if ((srcwidth > 0) && (srcheight > 0))
		{
			dst += x + dstwidth * y;
			for (int y = 0; y < srcheight; y++)
			{
				memcpy(dst, src, srcwidth * 4);
				dst += dstwidth, src += srcwidth;
			}
		}
	}
}