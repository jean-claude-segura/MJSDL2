#include "SDL_Tools.h"

void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees)
{
	SDL_Surface* invert = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	SDL_UpperBlit(src, 0, invert, 0);
	if(SDL_MUSTLOCK(invert))
		SDL_LockSurface(invert);

	Uint32* bufferZ = (Uint32*)invert->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < invert->h * invert->w; ++pixel, ++bufferZ)
		{
			/*
			Uint32 red = src->format->Rmask ^ *bufferZ & src->format->Rmask;
			Uint32 green = src->format->Gmask ^ *bufferZ & src->format->Gmask;
			Uint32 blue = src->format->Bmask ^ *bufferZ & src->format->Bmask;
			*bufferZ = red | green | blue | alpha;
			*/
			
			/*
			Uint32 alpha = *bufferZ & Amask;
			Uint32 rgb = RGBmask ^ *bufferZ & RGBmask;
			*bufferZ = rgb | alpha;
			*/
			
			*bufferZ = RGBmask - *bufferZ & RGBmask | *bufferZ & Amask;
		}
	}

	if (SDL_MUSTLOCK(invert))
		SDL_UnlockSurface(invert);

	SDL_UpperBlit(invert, NULL, dest, &coordonnees);
	SDL_FreeSurface(invert);
}

void SDL_UpperBlitXored(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees)
{
	SDL_Surface* xored = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	SDL_UpperBlit(src, 0, xored, 0);
	if (SDL_MUSTLOCK(xored))
		SDL_LockSurface(xored);

	Uint32* bufferZ = (Uint32*)xored->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < xored->h * xored->w; ++pixel, ++bufferZ)
		{
			/*
			Uint32 red = src->format->Rmask ^ *bufferZ & src->format->Rmask;
			Uint32 green = src->format->Gmask ^ *bufferZ & src->format->Gmask;
			Uint32 blue = src->format->Bmask ^ *bufferZ & src->format->Bmask;
			*bufferZ = red | green | blue | alpha;
			*/

			/*
			Uint32 alpha = *bufferZ & Amask;
			Uint32 rgb = RGBmask ^ *bufferZ & RGBmask;
			*bufferZ = rgb | alpha;
			*/

			* bufferZ = RGBmask ^ *bufferZ & RGBmask | *bufferZ & Amask;
		}
	}

	if (SDL_MUSTLOCK(xored))
		SDL_UnlockSurface(xored);

	SDL_UpperBlit(xored, NULL, dest, &coordonnees);
	SDL_FreeSurface(xored);
}

void SDL_UpperBlitNegate(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees)
{
	SDL_Surface* negate = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	SDL_UpperBlit(src, 0, negate, 0);
	if (SDL_MUSTLOCK(negate))
		SDL_LockSurface(negate);

	Uint32* bufferZ = (Uint32*)negate->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < negate->h * negate->w; ++pixel, ++bufferZ)
		{
			/*
			Uint32 red = src->format->Rmask ^ *bufferZ & src->format->Rmask;
			Uint32 green = src->format->Gmask ^ *bufferZ & src->format->Gmask;
			Uint32 blue = src->format->Bmask ^ *bufferZ & src->format->Bmask;
			*bufferZ = red | green | blue | alpha;
			*/

			/*
			Uint32 alpha = *bufferZ & Amask;
			Uint32 rgb = RGBmask ^ *bufferZ & RGBmask;
			*bufferZ = rgb | alpha;
			*/

			* bufferZ = ~*bufferZ & RGBmask | *bufferZ & Amask;
		}
	}

	if (SDL_MUSTLOCK(negate))
		SDL_UnlockSurface(negate);

	SDL_UpperBlit(negate, NULL, dest, &coordonnees);
	SDL_FreeSurface(negate);
}

void SDL_SetColourOnOpaque(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees, Uint32 ColourToFill)
{
	SDL_Surface* coloured = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	SDL_UpperBlit(src, 0, coloured, 0);
	if (SDL_MUSTLOCK(coloured))
		SDL_LockSurface(coloured);

	Uint32* bufferZ = (Uint32*)coloured->pixels;
	auto Amask = src->format->Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < coloured->h * coloured->w; ++pixel, ++bufferZ)
		{
			if ((*bufferZ & Amask))
				*bufferZ = ColourToFill | Amask;
		}
	}

	if (SDL_MUSTLOCK(coloured))
		SDL_UnlockSurface(coloured);
	
	//SDL_SetSurfaceBlendMode(coloured, SDL_BLENDMODE_NONE);
	SDL_UpperBlit(coloured, NULL, dest, &coordonnees);
	SDL_FreeSurface(coloured);
}

// https://tannerhelland.com/2011/10/01/grayscale-image-algorithm-vb6.html
// https://gigi.nullneuron.net/gigilabs/converting-an-image-to-grayscale-using-sdl2/
void SDL_SetGreyScale(SDL_Surface* src)
{
	// GIMP : Gray = (Red * 0.3 + Green * 0.59 + Blue * 0.11)
	// 0.3 * 256 + 0.59 * 256 + 0.11 * 256
	// (Red * 76.5 + Green * 150.45 + Blue * 28.05)
	// BT.709 : Gray = (Red * 0.2126 + Green * 0.7152 + Blue * 0.0722)
	// (Red * 54.213 + Green * 182.38875 + Blue * 18.411)
	// BT.601 : Gray = (Red * 0.299 + Green * 0.587 + Blue * 0.114)
	// (Red * 76.245 + Green * 149.685 + Blue * 29.07)
	if (SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	//src = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_ARGB8888, 0);

	Uint32* bufferZ = (Uint32*)src->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < src->h * src->w; ++pixel, ++bufferZ) //for (int pixel = 0; pixel < src->h * src->w; ++pixel, ++bufferZ)
		{
			auto colour = *bufferZ;
			auto r = (src->format->Rmask & *bufferZ) >> 16;
			auto g = (src->format->Gmask & *bufferZ) >> 8;
			auto b = src->format->Bmask & *bufferZ;
			//Uint8 avg = (77 * r + 150 * g + 29 * b) >> 8; // Org
			//Uint8 avg = (77 * r + 150 * g + 28 * b) >> 8; // GIMP
			Uint8 avg = (54 * r + 182 * g + 18 * b) >> 8; // BT.709
			//Uint8 avg = (76 * r + 150 * g + 29 * b) >> 8; // BT.601
			*bufferZ = avg << 16 | avg << 8 | avg | Amask;
		}
	}

	if (SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);
}

// Flip surface vertically.
void SDL_VerticalFlip(SDL_Surface* surface)
{
	SDL_LockSurface(surface);

	int pitch = surface->pitch; // row size
	char* temp = new char[pitch]; // intermediate buffer
	char* pixels = (char*)surface->pixels;

	for (int i = 0; i < surface->h >> 1; ++i) {
		// get pointers to the two rows to swap
		char* row1 = pixels + i * pitch;
		char* row2 = pixels + (surface->h - i - 1) * pitch;

		// swap rows
		memcpy(temp, row1, pitch);
		memcpy(row1, row2, pitch);
		memcpy(row2, temp, pitch);
	}

	delete[] temp;

	SDL_UnlockSurface(surface);
}

// Flip surface vertically.
void SDL_HorizontalFlip(SDL_Surface* surface)
{
	SDL_LockSurface(surface);

	Uint32* pixelsDeb = (Uint32*)surface->pixels;
	Uint32* pixelsFin = (Uint32*)surface->pixels + surface->w * surface->h - 1;
	Uint32 temp;

	for (int i = 0; i < (surface->w * surface->h) >> 1; ++i, ++pixelsDeb, --pixelsFin) {
		temp = *pixelsDeb;
		*pixelsDeb = *pixelsFin;
		*pixelsFin = temp;
	}

	SDL_UnlockSurface(surface);
}

void SDL_UpperBlitCut(SDL_Surface* src, SDL_Surface* dest)
{
	Uint32* bufferZ = (Uint32*)src->pixels;
	Uint32* bufferX = (Uint32*)dest->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < src->h * src->w; ++pixel, ++bufferZ, ++bufferX)
		{
			*bufferX = (*bufferX & RGBmask) | (*bufferZ & Amask);
		}
	}
}
