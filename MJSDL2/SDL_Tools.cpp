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

// Adapted from :
// https://lodev.org/cgtutor/fire.html
void FireTypeZero(Uint8 * fire, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
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
}

// Adapted from :
// https://lodev.org/cgtutor/fire.html
void FireTypeOne(Uint8* fire, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
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

void FireTypeTwo(Uint8* fire, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	// Adapted from :
	/* https://demo-effects.sourceforge.net/ */

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

// https://www.hanshq.net/fire.html
void FireTypeThree(SDL_Surface* firesurface, Uint8* fire, Uint8* prev_fire, Uint32* palette, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
	int i;
	uint32_t sum;
	uint8_t avg;

	for (i = SCREEN_WIDTH + 1; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH - 1; i++) {
		/* Average the eight neighbours. */
		sum = prev_fire[i - SCREEN_WIDTH - 1] +
			prev_fire[i - SCREEN_WIDTH] +
			prev_fire[i - SCREEN_WIDTH + 1] +
			prev_fire[i - 1] +
			prev_fire[i + 1] +
			prev_fire[i + SCREEN_WIDTH - 1] +
			prev_fire[i + SCREEN_WIDTH] +
			prev_fire[i + SCREEN_WIDTH + 1];
		avg = (uint8_t)(sum / 8);

		/* "Cool" the pixel if the two bottom bits of the
		   sum are clear (somewhat random). For the bottom
		   rows, cooling can overflow, causing "sparks". */
		if (!(sum & 3) &&
			(avg > 0 || i >= (SCREEN_HEIGHT - 4) * SCREEN_WIDTH)) {
			avg--;
		}
		fire[i] = avg;
	}

	/* Copy back and scroll up one row.
	   The bottom row is all zeros, so it can be skipped. */
	for (i = 0; i < (SCREEN_HEIGHT - 2) * SCREEN_WIDTH; i++) {
		prev_fire[i] = fire[i + SCREEN_WIDTH];
	}

	/* Remove dark pixels from the bottom rows (except again the
	   bottom row which is all zeros). */
	for (i = (SCREEN_HEIGHT - 7) * SCREEN_WIDTH; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
		if (fire[i] < 15) {
			fire[i] = 22 - fire[i];
		}
	}
	//set the drawing buffer to the fire buffer, using the palette colors
	auto p = (Uint32*)firesurface->pixels;
	/*
	Uint8* f = (Uint8*)fire;
	for(int Index = 0; Index < SCREEN_HEIGHT * SCREEN_WIDTH;  ++Index, ++p, ++f)
		*p = palette[*f];
	/**/
	/* Copy to framebuffer and map to RGBA, scrolling up one row. */
	/**/
	for (i = 0; i < (SCREEN_HEIGHT - 2) * SCREEN_WIDTH; i++) {
		p[i] = palette[fire[i + SCREEN_WIDTH]];
	}
	/**/
}

/*
To do or check if one of those does it :
(x, y) = ((x-1, y-1)+(x, y-1)+(x+1, y-1)) /3
(x, y) = ((x, y)+(x-1, y-1)+(x, y-1)+(x+1, y-1)) / 4
*/
void MakeFire(SDL_Surface* firesurface, Uint8* fire, Uint32* palette, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType)
{
	switch (FireType)
	{
	default:
		FireTypeZero(fire, size, SCREEN_WIDTH, SCREEN_HEIGHT);
		break;
	case 1:
		FireTypeOne(fire, size, SCREEN_WIDTH, SCREEN_HEIGHT);
		break;
	case 2:
		FireTypeTwo(fire, SCREEN_WIDTH, SCREEN_HEIGHT);
		break;
	}

	//set the drawing buffer to the fire buffer, using the palette colors
	auto p = (Uint32*)firesurface->pixels;
	Uint8* f = (Uint8*)fire;
	for(int index = 0; index < SCREEN_HEIGHT * SCREEN_WIDTH;  ++index, ++p, ++f)
		*p = palette[*f];
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

void SDL_FireOnTextureRectLinear(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);

	//auto test = HSLtoRGB(210, 0.79, 0.3);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	auto fire = new Uint8[SCREEN_HEIGHT * SCREEN_WIDTH];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));
	Uint8* prev_fire = NULL;
	if (FireType == 3)
	{
		prev_fire = new Uint8[SCREEN_HEIGHT * SCREEN_WIDTH];
		memset(prev_fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));
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

		if(FireType == 3)
			FireTypeThree(firesurface, fire, prev_fire, palette, size, SCREEN_WIDTH, SCREEN_HEIGHT);
		else
			MakeFire(firesurface, fire, palette, size, SCREEN_WIDTH, SCREEN_HEIGHT, FireType);

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

	if (FireType == 3)
		delete[] prev_fire;
	delete[] fire;
	delete[] palette;
}

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
			MakeFire(firesurfacet[t], firefox[t], palette, size, SCREEN_WIDTH, SCREEN_HEIGHT, FireType);
		}
			//draw the buffer and redraw the screen
		if (screen != NULL)
			SDL_RenderCopy(renderer, screen, NULL, NULL);
		for (int t = 0; t < 6; ++t)
		{
			auto texture = SDL_CreateTextureFromSurface(renderer, firesurfacet[t]);
			//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
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

void SDL_ExplosionOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_ExplosionOnTexture(renderer, renderTarget, (SDL_Texture*)NULL, Width, Height, NUMBER_OF_PARTICLES);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	auto screen = SDL_GetRenderTarget(renderer);
	SDL_ExplosionOnTexture(renderer, renderTarget, screen, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_ExplosionOnTextureRect(renderer, renderTarget, screen, NULL, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Adapted from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
void SDL_ExplosionOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);
	Uint32 buf, index, temp;
	int i, j;
	Uint8 * fire = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));

	PARTICLES particles(SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	//GenerateFireWithBluePalette(palette, size, Alpha);
	//GenerateBlueFirePalette(palette, size, Alpha);
	/*GenerateAnyHSLColourFirePalette(palette, size, uniform_int_dist_angle(e1), uniform_int_dist_angle(e1), Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;*/

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	bool bAtLeastOneAlive = false;

	auto start{ std::chrono::steady_clock::now() };
	auto end{ std::chrono::steady_clock::now() };
	std::chrono::duration<double> elapsed_seconds{end - start};

	//start the loop (one frame per loop)
	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds{end - start};
		if(elapsed_seconds.count() < 0.03)
			continue;
		start = std::chrono::steady_clock::now();
		if (!bAtLeastOneAlive)
		{
			GenerateAnyHSLColourFirePalette(palette, size, user_uniform_int_distribution<int>(0, 360), user_uniform_int_distribution<int>(0, 360), Alpha);
			for (int i = 0; i < size; ++i)
				if (palette[i] == Alpha << 24) palette[i] = 0;
			particles.init(NUMBER_OF_PARTICLES, PARTICLES::PARTICULES_TYPES::TYPE_RANDOMORIGIN);
		}

		/* move and draw particles into fire array */
		bAtLeastOneAlive = particles.draw(fire);

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

				((Uint32*)firesurface->pixels)[buf - SCREEN_WIDTH] = palette[temp];

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

void SDL_FireworkOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_FireworkOnTexture(renderer, renderTarget, (SDL_Texture*)NULL, Width, Height, NUMBER_OF_PARTICLES);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireworkOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	auto screen = SDL_GetRenderTarget(renderer);
	SDL_FireworkOnTexture(renderer, renderTarget, screen, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireworkOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_FireworkOnTextureRect(renderer, renderTarget, screen, NULL, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Adapted from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
void SDL_FireworkOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);
	Uint32 buf, index, temp;
	int i, j;
	Uint8* fire = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
	memset(fire, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));

	std::vector<PARTICLES::PARTICULES_TYPES> choices;
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_FORCEDORIGIN);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCULARDIR);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCULARPOS);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCLE);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_SPHERE);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_WATERFALL);

	PARTICLES particles(SCREEN_WIDTH, SCREEN_HEIGHT);
	TRAIL trail(SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	int size = 256;
	size = (size >> 1) << 1;
	Uint32* palette = new Uint32[size];
	//GenerateFireWithBluePalette(palette, size, Alpha);
	//GenerateBlueFirePalette(palette, size, Alpha);
	/*GenerateAnyHSLColourFirePalette(palette, size, uniform_int_dist_angle(e1), uniform_int_dist_angle(e1), Alpha);
	for (int i = 0; i < size; ++i)
		if (palette[i] == Alpha << 24) palette[i] = 0;*/

	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	bool bAtLeastOneAlive = false;
	bool bTrailAlive = false;

	auto start{ std::chrono::steady_clock::now() };
	auto end{ std::chrono::steady_clock::now() };

#ifdef _DEBUG
	Uint32 remaining = NUMBER_OF_PARTICLES;
#endif
	//start the loop (one frame per loop)
	while (true)
	{
		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds{end - start};
		if (elapsed_seconds.count() < 0.03)
			continue;
		start = std::chrono::steady_clock::now();

		if (!bAtLeastOneAlive)
		{
			trail.init();
			GenerateAnyHSLColourFirePalette(palette, size, user_uniform_int_distribution<int>(0, 360), user_uniform_int_distribution<int>(0, 360), Alpha);
			for (int i = 0; i < size; ++i)
				if (palette[i] == Alpha << 24) palette[i] = 0;
			bTrailAlive = true; // To start the trail.
			bAtLeastOneAlive = true; // To prevent init of the trail on next loop.
		}

		if (bTrailAlive)
		{
			bTrailAlive = false;
			trail.draw(fire, bTrailAlive);
			if (!bTrailAlive)
			{
#ifdef _DEBUG
				std::cout << "Trail died at : (" << trail.getXPos() << ";" << trail.getYPos() << ").";
				std::cout << " Screen is : (" << SCREEN_WIDTH << ";" << SCREEN_HEIGHT << ")." << std::endl;
				remaining = NUMBER_OF_PARTICLES;
#endif
				const int size = choices.size() - 1;
				auto next = user_uniform_int_distribution<int>(0, size);
				particles.init(NUMBER_OF_PARTICLES, choices[next], trail.getXPos(), trail.getYPos());
				//vParticles[i].init(NUMBER_OF_PARTICLES, PARTICLES::PARTICULES_TYPES::TYPE_THISISMADNESS, vTrails[i].getXPos(), vTrails[i].getYPos());
			}

		}
		else
		{
			/* move and draw particles into fire array */
			bAtLeastOneAlive = particles.draw(fire);
#ifdef _DEBUG
			Uint32 currentremaining = particles.getRemaining();
			if (remaining != currentremaining)
			{
				remaining = currentremaining;
				std::cout << remaining << " particles remaining." << std::endl;
			}
#endif
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

				((Uint32*)firesurface->pixels)[buf - SCREEN_WIDTH] = palette[temp];
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

void SDL_FireworksOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_FireworksOnTexture(renderer, renderTarget, (SDL_Texture*)NULL, Width, Height, NUMBER_OF_PARTICLES);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireworksOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	auto screen = SDL_GetRenderTarget(renderer);
	SDL_FireworksOnTexture(renderer, renderTarget, screen, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Current renderer must be set to the texture target and renderTarget to the main renderer texture
void SDL_FireworksOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_FireworksOnTextureRect(renderer, renderTarget, screen, NULL, Width, Height, NUMBER_OF_PARTICLES, Alpha);
}

// Adapted from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
void SDL_FireworksOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const uint32_t NUMBER_OF_PARTICLES, const Uint32 Alpha)
{
	SDL_SetRenderTarget(renderer, renderTarget);
	Uint32 buf, index, temp;

	const Uint8 max_number_of_fires = 15;
	Uint8 number_of_fires = 1;

	std::vector<PARTICLES::PARTICULES_TYPES> choices;
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_FORCEDORIGIN);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCULARDIR);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCULARPOS);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_CIRCLE);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_SPHERE);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_WATERFALL);
	choices.emplace_back(PARTICLES::PARTICULES_TYPES::TYPE_TRAIL);

	std::vector<PARTICLES> vParticles;
	std::vector<TRAIL> vTrails;

	bool * bAtLeastOneAlive = new bool [max_number_of_fires];
	bool * bTrailAlive = new bool[max_number_of_fires];

	SDL_Surface* firesurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	int size = 256;
	size = (size >> 1) << 1;
	Uint32** palette = new Uint32*[size];

	Uint8** fire = new Uint8*[max_number_of_fires];

	// Kept for deleters :
	for (Uint8 i = 0; i < max_number_of_fires; ++i)
	{
		palette[i] = NULL;

		fire[i] = NULL;
	}

	//for (Uint8 i = 0; i < max_number_of_fires; ++i)
	int i = 0;
	{
		vParticles.emplace_back(PARTICLES(SCREEN_WIDTH, SCREEN_HEIGHT));
		vTrails.emplace_back(TRAIL(SCREEN_WIDTH, SCREEN_HEIGHT));
		bAtLeastOneAlive[i] = false;
		bTrailAlive[i] = false;
		palette[i] = NULL;

		fire[i] = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
		memset(fire[i], 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));
	}


	SDL_Event event;
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);

	auto start{ std::chrono::steady_clock::now() };
	auto end{ std::chrono::steady_clock::now() };

	int limit = 5; // Why 2 and not a boolean? Because at start, the duration is not related to the actual rendering time.
	// Ok, so now why 5 and not 2? Well, because of the dynamic clocking, the cpu is not ready yet and the count is wrong.
	// But why 5? Well, I like this number, it's more than 2 and it seems it's high enough.

	//start the loop (one frame per loop)
	while (true)
	{
		end = std::chrono::steady_clock::now();

		if (SDL_PollEvent(&event) == 1 && (event.type == SDL_MOUSEBUTTONUP))
			break;

		// Read comment on limit = 2.
		std::chrono::duration<double> elapsed_seconds{end - start};
		if (limit > 0)
		{
			// Read comment on limit = 2;
			if (limit == 1 && elapsed_seconds.count() < 1. / 26. && number_of_fires != max_number_of_fires)
			{
				++number_of_fires;
				number_of_fires = std::min(number_of_fires, max_number_of_fires);
				int i = number_of_fires - 1;
				{
					vParticles.emplace_back(PARTICLES(SCREEN_WIDTH, SCREEN_HEIGHT));
					vTrails.emplace_back(TRAIL(SCREEN_WIDTH, SCREEN_HEIGHT));
					bAtLeastOneAlive[i] = false;
					bTrailAlive[i] = false;
					palette[i] = NULL;

					fire[i] = new Uint8[SCREEN_WIDTH * SCREEN_HEIGHT];
					memset(fire[i], 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint8));

					vTrails[i].init();
					if (palette[i] != NULL)
						delete[] palette[i];
					palette[i] = new Uint32[size];
					GenerateAnyHSLColourFirePalette(palette[i], size, user_uniform_int_distribution<int>(0, 360), user_uniform_int_distribution<int>(0, 360), Alpha);
					for (int c = 0; c < size; ++c)
						if (palette[i][c] == Alpha << 24) palette[i][c] = 0;
					bTrailAlive[i] = true; // To start the trail.
					bAtLeastOneAlive[i] = true; // To prevent init of the trail on next loop.
					const int size = choices.size() - 1;
					auto next = user_uniform_int_distribution<int>(0, size);
					vParticles[i].init(NUMBER_OF_PARTICLES, choices[next], vTrails[i].getXPos(), vTrails[i].getYPos());
				}
			}
			else
			{
				--limit;
#ifdef _DEBUG
				if(limit == 0)
					std::cout << "Final count of fireworks : " << int(number_of_fires) << "." << std::endl;
#endif
			}
		}
		/*else if (elapsed_seconds.count() < 1. / 26.)
		{
			continue;
		}*/

		start = std::chrono::steady_clock::now();

		for (int i = 0; i < number_of_fires; ++i)
		{
			if (!bAtLeastOneAlive[i])
			{
				vTrails[i].init();
				if (palette[i] != NULL)
					delete[] palette[i];
				palette[i] = new Uint32[size];
				GenerateAnyHSLColourFirePalette(palette[i], size, user_uniform_int_distribution<int>(0, 360), user_uniform_int_distribution<int>(0, 360), Alpha);
				for (int c = 0; c < size; ++c)
					if (palette[i][c] == Alpha << 24) palette[i][c] = 0;
				bTrailAlive[i] = true; // To start the trail.
				bAtLeastOneAlive[i] = true; // To prevent init of the trail on next loop.
			}

			if (bTrailAlive[i])
			{
				bTrailAlive[i] = false;
				vTrails[i].draw(fire[i], bTrailAlive[i]);
				if (!bTrailAlive[i])
				{
					const int size = choices.size() - 1;
					auto next = user_uniform_int_distribution<int>(0, size);
					vParticles[i].init(NUMBER_OF_PARTICLES, choices[next], vTrails[i].getXPos(), vTrails[i].getYPos());
					//vParticles[i].init(NUMBER_OF_PARTICLES, PARTICLES::PARTICULES_TYPES::TYPE_THISISMADNESS, vTrails[i].getXPos(), vTrails[i].getYPos());
					//vParticles[i].init(NUMBER_OF_PARTICLES, PARTICLES::PARTICULES_TYPES::TYPE_CIRCLE, vTrails[i].getXPos(), vTrails[i].getYPos());
				}
			}
			else
			{
				/* move and draw particles into fire[i] array */
				bAtLeastOneAlive[i] = vParticles[i].draw(fire[i]);
			}

			/* create fire[i] effect */
			for (int h = 1; h < SCREEN_HEIGHT - 2; ++h)
			{
				index = (h - 1) * SCREEN_WIDTH;

				for (int j = 1; j < SCREEN_WIDTH - 2; ++j)
				{
					buf = index + j;

					temp = fire[i][buf];
					temp += fire[i][buf + 1];
					temp += fire[i][buf - 1];
					buf += SCREEN_WIDTH;
					temp += fire[i][buf - 1];
					temp += fire[i][buf + 1];
					buf += SCREEN_WIDTH;
					temp += fire[i][buf];
					temp += fire[i][buf + 1];
					temp += fire[i][buf - 1];

					temp >>= 3;

					if (temp > 4)
					{
						temp -= 4;
					}
					else
					{
						temp = 0;
					}

					fire[i][buf - SCREEN_WIDTH] = temp;

					if (i == 0)
						((Uint32*)firesurface->pixels)[buf - SCREEN_WIDTH] = palette[i][temp];
					else
						((Uint32*)firesurface->pixels)[buf - SCREEN_WIDTH] |= palette[i][temp];
				}
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

	for (int z = 0; z < max_number_of_fires; ++z)
	{
		if (palette[z] != NULL)
			delete[] palette[z];
		if (fire[z] != NULL)
			delete[] fire[z];
	}
	delete[] fire;
	delete[] palette;
}
