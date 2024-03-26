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
				*bufferZ = ColourToFill;
		}
	}

	if (SDL_MUSTLOCK(coloured))
		SDL_UnlockSurface(coloured);
	
	SDL_SetSurfaceBlendMode(coloured, SDL_BLENDMODE_NONE);
	SDL_UpperBlit(coloured, NULL, dest, &coordonnees);
	SDL_FreeSurface(coloured);
}
