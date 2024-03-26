#include "SDL_Tools.h"

void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees)
{
	SDL_Surface* invert = SDL_CreateRGBSurface(0, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	SDL_UpperBlit(src, 0, invert, 0);
	if(SDL_MUSTLOCK(invert))
		SDL_LockSurface(invert);
	//std::cout << "Lock" << std::endl;
	Uint32* bufferZ = (Uint32*)invert->pixels;
	auto Amask = src->format->Amask;
	auto RGBmask = 0xFFFFFFFF & ~Amask;
	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < invert->h * invert->w; ++pixel, ++bufferZ)
		{
			/*Uint32 red = src->format->Rmask ^ *bufferZ & src->format->Rmask;
			Uint32 green = src->format->Gmask ^ *bufferZ & src->format->Gmask;
			Uint32 blue = src->format->Bmask ^ *bufferZ & src->format->Bmask;*/
			
			/*Uint32 alpha = *bufferZ & Amask;

			Uint32 rgb = RGBmask ^ *bufferZ & RGBmask;*/

			//*bufferZ = red | green | blue | alpha;

			//*bufferZ = rgb | alpha;

			*bufferZ = RGBmask ^ *bufferZ & RGBmask | *bufferZ & Amask;
		}
	}
	//std::cout << "Xored" << std::endl;
	if (SDL_MUSTLOCK(invert))
		SDL_UnlockSurface(invert);
	//std::cout << "Unlock" << std::endl;
	SDL_UpperBlit(invert, NULL, dest, &coordonnees);
	SDL_FreeSurface(invert);
}
