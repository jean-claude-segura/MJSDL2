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

void SDL_SetColourOnOpaque(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees, const Uint32 ColourToFill)
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
	// (Red * 76.8 + Green * 151.04 + Blue * 28.16)
	// BT.709 : Gray = (Red * 0.2126 + Green * 0.7152 + Blue * 0.0722)
	// (Red * 55.296 + Green * 183.0912 + Blue * 18.4832)
	// BT.601 : Gray = (Red * 0.299 + Green * 0.587 + Blue * 0.114)
	// (Red * 76.544 + Green * 150.272 + Blue * 29.184)
	if (SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	//src = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_ARGB8888, 0);

	Uint32* bufferZ = (Uint32*)src->pixels;
	auto Amask = src->format->Amask;

	if (src->format->BitsPerPixel == 32)
	{
		for (int pixel = 0; pixel < src->h * src->w; ++pixel, ++bufferZ) //for (int pixel = 0; pixel < src->SCREEN_HEIGHT * src->SCREEN_WIDTH; ++pixel, ++bufferZ)
		{
			auto colour = *bufferZ;
			auto r = (src->format->Rmask & *bufferZ) >> 16;
			auto g = (src->format->Gmask & *bufferZ) >> 8;
			auto b = src->format->Bmask & *bufferZ;
			//Uint8 avg = (77 * r + 150 * g + 29 * b) >> 8; // Org
			//Uint8 avg = (77 * r + 151 * g + 28 * b) >> 8; // GIMP
			Uint8 avg = (55 * r + 183 * g + 18 * b) >> 8; // BT.709
			//Uint8 avg = (77 * r + 150 * g + 29 * b) >> 8; // BT.601
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

void SDL_FireOnRenderer(SDL_Renderer * renderer, const int Width, const int Height, const int FireType)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_FireOnTexture(renderer, renderTarget, (SDL_Texture * )NULL, Width, Height, FireType);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const int FireType, const Uint32 Alpha)
{
	auto screen = SDL_GetRenderTarget(renderer);
	SDL_FireOnTexture(renderer, renderTarget, screen, Width, Height, FireType, Alpha);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const int FireType, const Uint32 Alpha)
{
	SDL_FireOnTextureRect(renderer, renderTarget, screen, NULL, Width, Height, FireType, Alpha);
}

// Adapted from :
// https://lodev.org/cgtutor/fire.html
void SDL_FireOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect * tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);

	//auto test = HSLtoRGB(210, 0.79, 0.3);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	auto fire = new Uint8 * [SCREEN_HEIGHT];
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		fire[i] = new Uint8[SCREEN_WIDTH];
		//make sure the fire buffer is zero in the beginning
		Uint8* f = fire[i];
		memset(f, 0, SCREEN_WIDTH * sizeof(Uint8));
	}

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	GenerateFirePalette(palette, size, Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	//start the loop (one frame per loop)
	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		/*
		*  ____________
		* |0,0.........|
		* |............|
		* |............|
		* |________x,_y|
		*/
		//randomize the bottom row of the fire buffer
		for (int x = 0; x < SCREEN_WIDTH; ++x) fire[SCREEN_HEIGHT - 1][x] = abs(32768 + rand()) % size;
		//do the fire calculations for every pixel, from top to bottom
		if (FireType == 0)
		{
			for (int y = 0; y < SCREEN_HEIGHT - 1; ++y)
			{
				for (int x = 0; x < SCREEN_WIDTH; ++x)
				{
					fire[y][x] =
						(
							(
								fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
								fire[y + 1][(x + 1) % SCREEN_WIDTH] +
								fire[(y + 2) % SCREEN_HEIGHT][x] +
								fire[y + 1][x]
							)
							* (size >> 3)) / (1 + ( size >> 1)
						);
				}
			}
		}
		else
		{
			for (int y = 0; y < SCREEN_HEIGHT - 1; ++y)
			{
				for (int x = 0; x < SCREEN_WIDTH; ++x)
				{
					fire[y][x] =
						(
							(
								fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
								fire[y + 1][(x + 1) % SCREEN_WIDTH] +
								fire[(y + 2) % SCREEN_HEIGHT][x] +
								fire[(y + 3) % SCREEN_HEIGHT][x]
							)
							* (size >> 2)
						)
						/ (1 + size);
				}
			}
		}
		//set the drawing buffer to the fire buffer, using the palette colors
		auto p = (Uint32*)firesurface->pixels;
		for (int y = 0; y < SCREEN_HEIGHT; ++y)
		{
			Uint8* f = fire[y];
			for (int x = 0; x < SCREEN_WIDTH; ++x, ++p, ++f)
			{
				*p = palette[*f];
			}
		}

		//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		auto texture = SDL_CreateTextureFromSurface(renderer, firesurface);
		SDL_RenderCopy(renderer, texture, NULL, tgtRect);
		SDL_DestroyTexture(texture);
		SDL_RenderPresent(renderer);
	}
	SDL_FreeSurface(firesurface);
	if (screen != NULL)
		SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_SetRenderTarget(renderer, renderTarget);

	for (int i = 0; i < SCREEN_HEIGHT; ++i) {
		delete [] fire[i];
	}
	delete [] fire;
	delete[] palette;
}

// Adapted from :
// https://lodev.org/cgtutor/fire.html
/* https://demo-effects.sourceforge.net/ */
void SDL_FireOnTextureRectLinear(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);

	//auto test = HSLtoRGB(210, 0.79, 0.3);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	auto fire = new Uint8[SCREEN_HEIGHT * SCREEN_WIDTH];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	GenerateFirePalette(palette, size, Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	//start the loop (one frame per loop)
	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		/*
		*  ____________
		* |0,0.........|
		* |............|
		* |............|
		* |________x,_y|
		*/
		switch (FireType)
		{
		default:
		case 0:
		{
			int FSIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
			//randomize the bottom row of the fire buffer
			for (int x = 0; x < SCREEN_WIDTH; ++x) fire[FSIZE - SCREEN_WIDTH + x] = abs(32768 + rand()) % size;
			//do the fire calculations for every pixel, from top to bottom
			for (int y = 0; y < FSIZE - SCREEN_WIDTH; y += SCREEN_WIDTH)
			{
				for (int x = 0; x < SCREEN_WIDTH; ++x)
				{
					fire[y + x] =
						(
							(
								fire[y + SCREEN_WIDTH + ((x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH)] +	// fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
								fire[y + SCREEN_WIDTH + ((x + 1) % SCREEN_WIDTH)] +					// fire[y + 1][(x + 1) % SCREEN_WIDTH] +
								fire[((y + 2 * SCREEN_WIDTH) % FSIZE) + x] +						// fire[(y + 2) % SCREEN_HEIGHT][x] +
								fire[y + SCREEN_WIDTH + x]											// fire[y + 1][x] +
								)
							* (size >> 3)) / (1 + (size >> 1)
								);
				}
			}
		}
		break;
		case 1:
		{
			int FSIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
			//randomize the bottom row of the fire buffer
			for (int x = 0; x < SCREEN_WIDTH; ++x) fire[FSIZE - SCREEN_WIDTH + x] = abs(32768 + rand()) % size;
			//do the fire calculations for every pixel, from top to bottom
			for (int y = 0; y < FSIZE - SCREEN_WIDTH; y += SCREEN_WIDTH)
			{
				for (int x = 0; x < SCREEN_WIDTH; ++x)
				{
					fire[y + x] =
						(
							(
								fire[y + SCREEN_WIDTH + ((x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH)] +	// fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
								fire[y + SCREEN_WIDTH + ((x + 1) % SCREEN_WIDTH)] +					// fire[y + 1][(x + 1) % SCREEN_WIDTH] +
								fire[((y + 2 * SCREEN_WIDTH) % FSIZE) + x] +						// fire[(y + 2) % SCREEN_HEIGHT][x] +
								fire[((y + 3 * SCREEN_WIDTH) % FSIZE) + x]							// fire[(y + 3) % SCREEN_HEIGHT][x]
								)
							* (size >> 2)
							)
						/ (1 + size);
				}
			}
		}
		break;
		case 2:
		{
			int j = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
			for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
			{
				// 0 <= rand() <= RAND_MAX
				// 0  / (RAND_MAX + 1.0) >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
				// 0 >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
				int random = 1 + (int)(16.0 * (rand() / (RAND_MAX + 1.0)));
				if (random > 9) // the lower the value, the intenser the fire, compensate a lower value with a higher decay value
					fire[j + i] = 255; // maximum heat
				else
					fire[j + i] = 0;
			}

			/* move fire upwards, start at bottom*/
			Uint16 temp;

			for (int index = 0; index < 60; ++index) // Nombre de passes. Max : SCREEN_HEIGHT - 1
			{
				for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
				{
					/*if (i == 0) // at the left border
					{
						temp = fire[j];
						temp += fire[j + 1];
						temp += fire[j - SCREEN_WIDTH];
						temp /= 3;
					}
					else if (i == SCREEN_WIDTH - 1) // at the right border
					{
						temp = fire[j + i];
						temp += fire[j - SCREEN_WIDTH + i];
						temp += fire[j + i - 1];
						temp /= 3;
					}
					else*/
					{
						temp = fire[j + i];
						temp += fire[j + i + 1];
						temp += fire[j + i - 1];
						temp += fire[j - SCREEN_WIDTH + i];
						temp >>= 2;
					}
					if (temp > 1)
						temp -= 1; /* decay */

					fire[j - SCREEN_WIDTH + i] = temp;
				}
				j -= SCREEN_WIDTH;
			}
		}
		break;
		}

		//set the drawing buffer to the fire buffer, using the palette colors
		auto p = (Uint32*)firesurface->pixels;
		Uint8* f = (Uint8*)fire;
		for (int y = 0; y < SCREEN_HEIGHT; ++y)
		{
			for (int x = 0; x < SCREEN_WIDTH; ++x, ++p, ++f)
			{
				*p = palette[*f];
			}
		}

		//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		auto texture = SDL_CreateTextureFromSurface(renderer, firesurface);
		SDL_RenderCopy(renderer, texture, NULL, tgtRect);
		SDL_DestroyTexture(texture);
		SDL_RenderPresent(renderer);
	}
	SDL_FreeSurface(firesurface);
	if (screen != NULL)
		SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_SetRenderTarget(renderer, renderTarget);

	delete[] fire;
	delete[] palette;
}

// Adapted from :
/* https://demo-effects.sourceforge.net/ */
void SDL_FireOnTextureBisRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	int size = 256;
	size = (size >> 1) << 1;

	Uint32 * palette = new Uint32[size];

	GenerateFireWithBluePalette(palette, size);
	//GenerateFirePalette(palette, size, 0xC0);

	//for (int i = 0; i < size; ++i) if(palette[i] != 0 ) palette[i] |= 0xFF000000;
	//for (int i = 0; i < size; ++i) palette[i] |= 0xFF000000;
	//for (int i = 0; i < size; ++i) if(palette[i] == 0 ) palette[i] = 0xFFFFFFFF;
	//for (int i = 0; i < size; ++i) if(palette[i] == 0xC0000000 ) palette[i] = 0;

	//make sure the fire buffer is zero in the beginning
	Uint8* fire = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));

	SDL_Event event;
	//start the loop (one frame per loop)
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		/*
		*  ____________
		* |0,0.........|
		* |............|
		* |............|
		* |________x,_y|
		*/
		int j = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
		for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
		{
			// 0 <= rand() <= RAND_MAX
			// 0  / (RAND_MAX + 1.0) >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
			// 0 >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
			int random = 1 + (int)(16.0 * (rand() / (RAND_MAX + 1.0)));
			if (random > 9) // the lower the value, the intenser the fire, compensate a lower value with a higher decay value
				fire[j + i] = 255; // maximum heat
			else
				fire[j + i] = 0;
		}

		/*
		*  ____________
		* |0,0.........|
		* |............|
		* |............|
		* |________x,_y|
		*/
		/* move fire upwards, start at bottom*/
		Uint16 temp;

		for (int index = 0; index < 60; ++index) // Nombre de passes. Max : SCREEN_HEIGHT - 1
		{
			for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
			{
				/*if (i == 0) // at the left border
				{
					temp = fire[j];
					temp += fire[j + 1];
					temp += fire[j - SCREEN_WIDTH];
					temp /= 3;
				}
				else if (i == SCREEN_WIDTH - 1) // at the right border
				{
					temp = fire[j + i];
					temp += fire[j - SCREEN_WIDTH + i];
					temp += fire[j + i - 1];
					temp /= 3;
				}
				else*/
				{
					temp = fire[j + i];
					temp += fire[j + i + 1];
					temp += fire[j + i - 1];
					temp += fire[j - SCREEN_WIDTH + i];
					temp >>= 2;
				}
				if (temp > 1)
					temp -= 1; /* decay */

				fire[j - SCREEN_WIDTH + i] = temp;
			}
			j -= SCREEN_WIDTH;
		}

		//set the drawing buffer to the fire buffer, using the palette colors
		auto fireprime = fire;
		auto p = (Uint32*)firesurface->pixels;
		for ( ; p <= (Uint32*)firesurface->pixels + (SCREEN_WIDTH * SCREEN_HEIGHT) - 1; ++p, ++fireprime)
		{
			*p = palette[*fireprime];
		}
		
		//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		auto texture = SDL_CreateTextureFromSurface(renderer, firesurface);
		SDL_RenderCopy(renderer, texture, NULL, tgtRect);
		SDL_DestroyTexture(texture);
		SDL_RenderPresent(renderer);
	}
	delete [] fire;
	delete [] palette;
	SDL_FreeSurface(firesurface);
	if (screen != NULL)
		SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_SetRenderTarget(renderer, renderTarget);
}

// Adapted from :
// https://lodev.org/cgtutor/fire.html
/* https://demo-effects.sourceforge.net/ */
void SDL_FireOnTilesRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);

	//auto test = HSLtoRGB(210, 0.79, 0.3);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_Surface** firesurfacet = new SDL_Surface * [6];
	for (int i = 0; i < 6; ++i)
		firesurfacet[i] = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	auto firefox = new Uint8 * [6];
	for (int i = 0; i < 6; ++i)
	{
		firefox[i] = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
		//make sure the fire buffer is zero in the beginning
		Uint8* f = firefox[i];
		memset(f, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));
	}

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	GenerateFirePalette(palette, size, Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	//start the loop (one frame per loop)
	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		/*
		*  ____________
		* |0,0.........|
		* |............|
		* |............|
		* |________x,_y|
		*/
		//randomize the bottom row of the fire buffer
		for (int t = 0; t < 6; ++t)
		{
			auto fire = firefox[t];
			/*
			*  ____________
			* |0,0.........|
			* |............|
			* |............|
			* |________x,_y|
			*/
			switch (FireType)
			{
			default:
			case 0:
			{
				int FSIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
				//randomize the bottom row of the fire buffer
				for (int x = 0; x < SCREEN_WIDTH; ++x) fire[FSIZE - SCREEN_WIDTH + x] = abs(32768 + rand()) % size;
				//do the fire calculations for every pixel, from top to bottom
				for (int y = 0; y < FSIZE - SCREEN_WIDTH; y += SCREEN_WIDTH)
				{
					for (int x = 0; x < SCREEN_WIDTH; ++x)
					{
						fire[y + x] =
							(
								(
									fire[y + SCREEN_WIDTH + ((x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH)] +	// fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
									fire[y + SCREEN_WIDTH + ((x + 1) % SCREEN_WIDTH)] +					// fire[y + 1][(x + 1) % SCREEN_WIDTH] +
									fire[((y + 2 * SCREEN_WIDTH) % FSIZE) + x] +						// fire[(y + 2) % SCREEN_HEIGHT][x] +
									fire[y + SCREEN_WIDTH + x]											// fire[y + 1][x] +
									)
								* (size >> 3)) / (1 + (size >> 1)
									);
					}
				}
			}
			break;
			case 1:
			{
				int FSIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
				//randomize the bottom row of the fire buffer
				for (int x = 0; x < SCREEN_WIDTH; ++x) fire[FSIZE - SCREEN_WIDTH + x] = abs(32768 + rand()) % size;
				//do the fire calculations for every pixel, from top to bottom
				for (int y = 0; y < FSIZE - SCREEN_WIDTH; y += SCREEN_WIDTH)
				{
					for (int x = 0; x < SCREEN_WIDTH; ++x)
					{
						fire[y + x] =
							(
								(
									fire[y + SCREEN_WIDTH + ((x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH)] +	// fire[y + 1][(x - 1 + SCREEN_WIDTH) % SCREEN_WIDTH] +
									fire[y + SCREEN_WIDTH + ((x + 1) % SCREEN_WIDTH)] +					// fire[y + 1][(x + 1) % SCREEN_WIDTH] +
									fire[((y + 2 * SCREEN_WIDTH) % FSIZE) + x] +						// fire[(y + 2) % SCREEN_HEIGHT][x] +
									fire[((y + 3 * SCREEN_WIDTH) % FSIZE) + x]							// fire[(y + 3) % SCREEN_HEIGHT][x]
									)
								* (size >> 2)
								)
							/ (1 + size);
					}
				}
			}
			break;
			case 2:
			{
				int j = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
				for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
				{
					// 0 <= rand() <= RAND_MAX
					// 0  / (RAND_MAX + 1.0) >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
					// 0 >= rand()  / (RAND_MAX + 1.0) >= RAND_MAX  / (RAND_MAX + 1.0)
					int random = 1 + (int)(16.0 * (rand() / (RAND_MAX + 1.0)));
					if (random > 9) // the lower the value, the intenser the fire, compensate a lower value with a higher decay value
						fire[j + i] = 255; // maximum heat
					else
						fire[j + i] = 0;
				}

				/* move fire upwards, start at bottom*/
				Uint16 temp;

				for (int index = 0; index < 60; ++index) // Nombre de passes. Max : SCREEN_HEIGHT - 1
				{
					for (int i = 0; i < SCREEN_WIDTH - 1; ++i)
					{
						/*if (i == 0) // at the left border
						{
							temp = fire[j];
							temp += fire[j + 1];
							temp += fire[j - SCREEN_WIDTH];
							temp /= 3;
						}
						else if (i == SCREEN_WIDTH - 1) // at the right border
						{
							temp = fire[j + i];
							temp += fire[j - SCREEN_WIDTH + i];
							temp += fire[j + i - 1];
							temp /= 3;
						}
						else*/
						{
							temp = fire[j + i];
							temp += fire[j + i + 1];
							temp += fire[j + i - 1];
							temp += fire[j - SCREEN_WIDTH + i];
							temp >>= 2;
						}
						if (temp > 1)
							temp -= 1; /* decay */

						fire[j - SCREEN_WIDTH + i] = temp;
					}
					j -= SCREEN_WIDTH;
				}
			}
			break;
			}
		}
		//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		for (int t = 0; t < 6; ++t)
		{
			//set the drawing buffer to the fire buffer, using the palette colors
			auto p = (Uint32*)firesurfacet[t]->pixels;
			auto f = firefox[t];
			for (int y = 0; y < SCREEN_HEIGHT; ++y)
			{
				for (int x = 0; x < SCREEN_WIDTH; ++x, ++p, ++f)
				{
					*p = palette[*f];
				}
			}
			auto texture = SDL_CreateTextureFromSurface(renderer, firesurfacet[t]);
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_Rect r;
			r.x = tgtRect->x;
			r.y = tgtRect->y + t * (tgtRect->h - 40);
			r.w = tgtRect->w;
			r.h = tgtRect->h;
			SDL_RenderCopy(renderer, texture, NULL, &r);
			SDL_DestroyTexture(texture);
		}

		SDL_RenderPresent(renderer);
	}

	SDL_FreeSurface(firesurface);
	for (int i = 0; i < 6; ++i)
		SDL_FreeSurface(firesurfacet[i]);
	if (screen != NULL)
		SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_SetRenderTarget(renderer, renderTarget);
	
	for (int i = 0; i < 6; ++i)
	{
		delete[] firefox[i];
	}
	delete [] firefox;

	delete[] palette;
}

void init_particle(PARTICLE* particle, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	/* randomly init particles, generate them in the center of the screen */

	particle->xpos = (SCREEN_WIDTH >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	particle->ypos = (SCREEN_HEIGHT >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	particle->xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	particle->ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
	particle->colorindex = 255;
	particle->dead = false;
}

void SDL_ExplosionOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const int NUMBER_OF_PARTICLES)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_ExplosionOnTexture(renderer, renderTarget, (SDL_Texture*)NULL, Width, Height, NUMBER_OF_PARTICLES);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const int NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	auto screen = SDL_GetRenderTarget(renderer);
	SDL_ExplosionOnTexture(renderer, renderTarget, screen, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const int NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_ExplosionOnTextureRect(renderer, renderTarget, screen, NULL, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Adapted from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
void SDL_ExplosionOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);
	Uint32 buf, index, temp;
	int i, j;
	Uint8 * fire = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));
	PARTICLE * particles = new PARTICLE[NUMBER_OF_PARTICLES];
	/*for (i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		init_particle(particles + i, SCREEN_WIDTH, SCREEN_HEIGHT);
	}*/

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	GenerateFireWithBluePalette(palette, size, Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	bool bAtLeastOneAlive = false;

	//start the loop (one frame per loop)
	while (true)
	{
		if (!bAtLeastOneAlive)
		{
			for (i = 0; i < NUMBER_OF_PARTICLES; ++i)
			{
				init_particle(particles + i, SCREEN_WIDTH, SCREEN_HEIGHT);
			}
		}

		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		/* move and draw particles into fire array */
		bAtLeastOneAlive = false;
		for (i = 0; i < NUMBER_OF_PARTICLES; ++i)
		{
			bAtLeastOneAlive |= !particles[i].dead;
			if (!particles[i].dead)
			{
				particles[i].xpos += particles[i].xdir;
				particles[i].ypos += particles[i].ydir;

				/* is particle dead? */

				if (particles[i].colorindex == 0 ||
					(particles[i].ypos >= SCREEN_HEIGHT - 3) ||
					particles[i].xpos <= 1 ||
					particles[i].xpos >= SCREEN_WIDTH - 3)
				{
					particles[i].dead = true;
					continue;
				}

				/* gravity takes over */
				particles[i].ydir++;

				/* particle cools off */
				particles[i].colorindex--;

				/* draw particle */
				temp = particles[i].ypos * SCREEN_WIDTH + particles[i].xpos;

				fire[temp] = particles[i].colorindex;
				fire[temp - 1] = particles[i].colorindex;
				fire[temp + SCREEN_WIDTH] = particles[i].colorindex;
				fire[temp - SCREEN_WIDTH] = particles[i].colorindex;
				fire[temp + 1] = particles[i].colorindex;
			}
		}

		/* create fire effect */
		for (i = 1; i < SCREEN_HEIGHT - 2; ++i)
		{
			index = (i - 1) * SCREEN_WIDTH;

			for (j = 1; j < SCREEN_WIDTH - 2; ++j)
			{
				buf = index + j;

				temp = fire[buf];
				temp += fire[buf + 1];
				temp += fire[buf - 1];
				buf += SCREEN_WIDTH;
				temp += fire[buf - 1];
				temp += fire[buf + 1];
				buf += SCREEN_WIDTH;
				temp += fire[buf];
				temp += fire[buf + 1];
				temp += fire[buf - 1];

				temp >>= 3;

				if (temp > 4)
				{
					temp -= 4;
				}
				else
				{
					temp = 0;
				}

				fire[buf - SCREEN_WIDTH] = temp;
			}
		}

		/* draw fire array to screen from bottom to top*/
		auto p = (Uint32*)firesurface->pixels;
		auto f = fire;
		for (int y = 0; y < SCREEN_HEIGHT; ++y)
		{
			for (int x = 0; x < SCREEN_WIDTH; ++x, ++p, ++f)
			{
				*p = palette[*f];
			}
		}

		//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		auto texture = SDL_CreateTextureFromSurface(renderer, firesurface);
		SDL_RenderCopy(renderer, texture, NULL, tgtRect);
		SDL_DestroyTexture(texture);
		SDL_RenderPresent(renderer);
	}

	SDL_FreeSurface(firesurface);
	if (screen != NULL)
		SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_SetRenderTarget(renderer, renderTarget);

	delete[] particles;
	delete[] fire;
	delete[] palette;
}
