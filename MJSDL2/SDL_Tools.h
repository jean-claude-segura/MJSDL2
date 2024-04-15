#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Tools.h"
#include <chrono>
#include <numbers>

void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitXored(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitNegate(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_SetColourOnOpaque(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees, const Uint32 ColourToFill);
void SDL_SetGreyScale(SDL_Surface* src);
void SDL_VerticalFlip(SDL_Surface* surface);
void SDL_HorizontalFlip(SDL_Surface* surface);
void SDL_UpperBlitCut(SDL_Surface* src, SDL_Surface* dest);

/* FIRES */
void FireTypeZero(Uint8* fire, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void FireTypeOne(Uint8* fire, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void FireTypeTwo(Uint8* fire, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void FireTypeThree(SDL_Surface* firesurface, Uint8* fire, Uint8* prev_fire, Uint32* palette, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void MakeFire(SDL_Surface* firesurface, Uint8* fire, Uint32* palette, const int size, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int FireType);
void SDL_FireOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const int FireType = 0);
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const int FireType = 0, const Uint32 Alpha = 0x00);
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const int FireType = 0, const Uint32 Alpha = 0x00);
void SDL_FireOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int Width, const int Height, const int FireType, const Uint32 Alpha);
void SDL_FireOnTextureRectLinear(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int Width, const int Height, const int FireType, const Uint32 Alpha);
void SDL_FireOnTilesRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int Width, const int Height, const int FireType, const Uint32 Alpha);

/* EXPLOSIONS */
/*particle structure*/
class PARTICLE
{
protected:
	int32_t xpos, ypos;
	int32_t xdir, ydir;
	Uint8 colorindex;
	bool dead;
	int32_t SCREEN_WIDTH;
	int32_t SCREEN_HEIGHT;
	virtual const bool setDeath() = 0;
	PARTICLE(const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT) : SCREEN_WIDTH(_SCREEN_WIDTH), SCREEN_HEIGHT(_SCREEN_HEIGHT)
	{
		xpos = ypos = 0;
		xdir = ydir = 0;
		colorindex = 255;
		dead = false;
	}
public:
	bool draw(Uint8 * fire, bool &bAtLeastOneAlive)
	{
		if (!dead && !setDeath())
		{
			int32_t temp;
			/* draw particle */
			if (ypos > 1 && xpos > 1 &&
				(xpos < SCREEN_WIDTH - 3))
			{
				temp = ypos * SCREEN_WIDTH + xpos;

				fire[temp] = colorindex;
				fire[temp - 1] = colorindex;
				fire[temp + SCREEN_WIDTH] = colorindex;
				fire[temp - SCREEN_WIDTH] = colorindex;
				fire[temp + 1] = colorindex;
			}
			bAtLeastOneAlive = true;
			return true;
		}
		return false;
	}
};

class COMMON : public PARTICLE
{
protected:
	const bool setDeath()
	{
		xpos += xdir;
		ypos += ydir;

		/* is particle dead? */
		if (colorindex == 0 ||
			(ypos >= SCREEN_HEIGHT - 3))
		{
			dead = true;
			return true;
		}

		// Is particle on the left side of visible screen coming back ?
		// If not -> dead.
		if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
		{
			dead = true;
			return true;
		}

		// Is particle on the right side of visible screen coming back ?
		// If not -> dead.
		if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
		{
			dead = true;
			return true;
		}

		// Is particle above (Actually *under*) visible screen coming back ?
		// If not -> dead.
		if (ypos <= 1)
		{
			int32_t deltaX = xdir;
			if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
				(deltaX < 0 && (xpos + deltaX <= 1)))
			{
				dead = true;
				return true;
			}
		}

		/* gravity takes over */
		ydir++;

		/* particle cools off */
		colorindex--;

		return false;
	}

	COMMON(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT) {};
};

class RANDOMORIGIN : public COMMON
{
public:
	RANDOMORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : COMMON(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
	}
};

class CENTEREDEDORIGIN : public COMMON
{
public:
	CENTEREDEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : COMMON(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		/* randomly init particle, generate it in the center of the screen */
		xpos = (SCREEN_WIDTH >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = (SCREEN_HEIGHT >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
	}
};

class FORCEDORIGIN : public COMMON
{
public:
	FORCEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : COMMON(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
	}
};

class CIRCULARPOS : public COMMON
{
private:
	const double radius;
public:
	CIRCULARPOS(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double _radius) : radius(_radius), COMMON(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		double angle = std::numbers::pi * 2 * (rand() / (RAND_MAX + 1.0));
		xpos = xOrg + std::cos(angle) * radius; // X est le cosinus de l'angle.
		ypos = yOrg + std::sin(angle) * radius; // Y est le sinus de l'angle.
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
	}
};

class CIRCULARDIR : public COMMON
{
public:
	CIRCULARDIR(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : COMMON(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		double angle = std::numbers::pi * 2 * (rand() / (RAND_MAX + 1.0));
		xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		xdir = std::cos(angle) * 10;
		ydir = std::sin(angle) * 10;
	}
};

class TRAIL : public PARTICLE
{
public:
	TRAIL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		xpos = (int)(SCREEN_WIDTH * (rand() / (RAND_MAX + 1.0))) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = SCREEN_HEIGHT - 4;
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -32 + (int)(12.0 * (rand() / (RAND_MAX + 1.0))); // -33 <  < -20
	}

	void init()
	{
		xpos = (int)(SCREEN_WIDTH * (rand() / (RAND_MAX + 1.0))) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
		ypos = SCREEN_HEIGHT - 4;
		xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
		ydir = -32 + (int)(12.0 * (rand() / (RAND_MAX + 1.0))); // -32 -20
		colorindex = 255;
		dead = false;
	}

	const int32_t getXPos() { return xpos; }
	const int32_t getYPos() { return ypos; }

protected:
	const bool setDeath()
	{
		xpos += xdir;
		ypos += ydir;

		/* is particle dead? */
		if (colorindex == 0 ||
			(ypos >= SCREEN_HEIGHT - 3))
		{
			dead = true;
			return true;
		}

		// Is particle above (Actually *under*) visible screen coming back ?
		// If not -> dead.
		if (ypos <= 1)
		{
			int32_t deltaX = xdir;
			if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
				(deltaX < 0 && (xpos + deltaX <= 1)))
			{
				dead = true;
				return true;
			}
		}

		if (ydir > 0)
		{
			dead = true;
			return true;
		}

		/* gravity takes over */
		ydir++;

		/* particle cools off */
		colorindex--;
		return false;
	}
};

class CIRCLE : public CIRCULARDIR
{
public:
	CIRCLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : CIRCULARDIR(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
	{
		radius = 30;
	}
private:
	Uint8 radius;
protected:
	const bool setDeath()
	{
		xpos += xdir;
		ypos += ydir;
		if(radius > 0)
			--radius;

		/* is particle dead? */
		if (colorindex == 0 ||
			(ypos >= SCREEN_HEIGHT - 3) ||
			radius == 0)
		{
			dead = true;
			return true;
		}

		// Is particle on the left side of visible screen coming back ?
		// If not -> dead.
		if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
		{
			dead = true;
			return true;
		}

		// Is particle on the right side of visible screen coming back ?
		// If not -> dead.
		if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
		{
			dead = true;
			return true;
		}

		if (true)
		{
			// Is particle above (Actually *under*) visible screen coming back ?
			// If not -> dead.
			if (ypos <= 1 && ydir <= 0) // Circulars never fall.
			{
				dead = true;
				return true;
			}
		}
		else
		{
			// Is particle above (Actually *under*) visible screen coming back ?
			// If not -> dead.
			if (ypos <= 1)
			{
				int32_t deltaX = xdir;
				if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
					(deltaX < 0 && (xpos + deltaX <= 1)))
				{
					dead = true;
					return true;
				}
			}
		}

		/* gravity takes over */
		if(false && radius == 0)
			ydir++;

		/* particle cools off */
		colorindex--;

		return false;
	}
};


void SDL_ExplosionOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500);
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500, const Uint32 Alpha = 0x00);
void SDL_ExplosionOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500, const Uint32 Alpha = 0x00);
void SDL_ExplosionOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int Width, const int Height, const int NUMBER_OF_PARTICLES, const Uint32 Alpha);
	
void SDL_FireworkOnRenderer(SDL_Renderer* renderer, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500);
void SDL_FireworkOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500, const Uint32 Alpha = 0x00);
void SDL_FireworkOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, const int Width, const int Height, const int NUMBER_OF_PARTICLES = 500, const Uint32 Alpha = 0x00);
void SDL_FireworkOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, const int Width, const int Height, const int NUMBER_OF_PARTICLES, const Uint32 Alpha);

/* *********************************************************************************** /
/*                                       INLINE                                       */
/* ********************************************************************************** */

/* ************************************************************************************************************************** /
* https://discourse.libsdl.org/t/sdl-composecustomblendmode-error-in-windows/35241/1                                          /
* ************************************************************************************************************************** */
inline SDL_Texture* SDL_InvertTexture(SDL_Renderer* renderer, SDL_Texture* src, SDL_Texture*& tgt)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	auto SDLRenderer = renderer;
	{
		int w, h;
		SDL_BlendMode textureBlendMode;
		SDL_GetTextureBlendMode(src, &textureBlendMode);
		if (SDL_SetTextureBlendMode(src, SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
			SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD)) == 0)
		{
			if (tgt != NULL) SDL_DestroyTexture(tgt);
			SDL_QueryTexture(src, NULL, NULL, &w, &h);
			tgt = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
			SDL_SetRenderTarget(SDLRenderer, tgt);
			SDL_SetRenderDrawColor(SDLRenderer, 0xFF, 0xFF, 0xFF, 0);
			SDL_RenderClear(SDLRenderer);
			SDL_RenderCopy(SDLRenderer, src, NULL, NULL);
			SDL_SetTextureBlendMode(tgt, textureBlendMode);
		}
		SDL_SetTextureBlendMode(src, textureBlendMode);
	}
	SDL_SetRenderTarget(renderer, renderTarget);
	return tgt;
}

/* ************************************************************************************************************************** /
* https://stackoverflow.com/questions/75873908/how-to-copy-a-texture-to-another-texture-without-pointing-to-the-same-texture  /
* ************************************************************************************************************************** */
inline SDL_Texture* SDL_DuplicateTexture(SDL_Renderer* renderer, SDL_Texture* src, SDL_Texture*& tgt)
{
	Uint32 format;
	int w, h;
	SDL_BlendMode blendmode;

	// Save the current rendering target (will be NULL if it is the current window)
	auto renderTarget = SDL_GetRenderTarget(renderer);

	// Get all properties from the texture we are duplicating
	SDL_QueryTexture(src, &format, NULL, &w, &h);
	SDL_GetTextureBlendMode(src, &blendmode);

	// Create a new texture with the same properties as the one we are duplicating
	if (tgt != NULL)
		SDL_DestroyTexture(tgt);
	tgt = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);

	// Set its blending mode and make it the render target
	SDL_SetTextureBlendMode(tgt, SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(renderer, tgt);

	// Render the full original texture onto the new one
	SDL_RenderCopy(renderer, src, NULL, NULL);

	// Change the blending mode of the new texture to the same as the original one
	SDL_SetTextureBlendMode(tgt, blendmode);

	// Restore the render target
	SDL_SetRenderTarget(renderer, renderTarget);

	// Return the new texture
	return tgt;
}

inline SDL_Texture* SDL_CutTextureOnAlpha(SDL_Renderer* renderer, SDL_Texture* src, SDL_Texture* tgt)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	auto SDLRenderer = renderer;
	{
		SDL_BlendMode textureBlendMode;
		SDL_GetTextureBlendMode(src, &textureBlendMode);
		if (SDL_SetTextureBlendMode(src, SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_MINIMUM)) == 0)
		{
			SDL_SetRenderTarget(SDLRenderer, tgt);
			SDL_RenderCopy(SDLRenderer, src, NULL, NULL);
			SDL_SetTextureBlendMode(tgt, textureBlendMode);
		}
		SDL_SetTextureBlendMode(src, textureBlendMode);
	}
	SDL_SetRenderTarget(renderer, renderTarget);
	return tgt;
}

inline Uint32 SDL_TextureReadPixel(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Point p, const int WIDTH)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_Rect textRec;
	textRec.x = p.x;
	textRec.y = p.y;
	textRec.w = 1;
	textRec.h = 1;
	Uint32 value;
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderReadPixels(renderer, &textRec, SDL_PIXELFORMAT_ARGB8888, &value, 32 * WIDTH);
	SDL_SetRenderTarget(renderer, renderTarget);
	return value;
}

/* ***************************************************************************************************************************************************************************************** /
* https://cpp.hotexamples.com/examples/-/-/SDL_RenderReadPixels/cpp-sdl_renderreadpixels-function-examples.html#0xa84d0ea8abf09c741fc17c5a0ebb53d9368f9ad77c3ed927824fea5338e7bb5c-152,,162, /
* ***************************************************************************************************************************************************************************************** */
inline SDL_Texture* SDL_GreyscaleTexture(SDL_Renderer* renderer, SDL_Texture* src, SDL_Texture*& tgt)
{
	SDL_DuplicateTexture(renderer, src, tgt);

	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, src);

	Uint32 format;
	int w, h;
	SDL_QueryTexture(src, &format, NULL, &w, &h);
	SDL_Surface* screenshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

	Uint32* buffer = (Uint32*)screenshot->pixels;
	auto Amask = screenshot->format->Amask;

	for (int pixel = 0; pixel < screenshot->h * screenshot->w; ++pixel, ++buffer)
	{
		auto colour = *buffer;
		auto r = (screenshot->format->Rmask & *buffer) >> 16;
		auto g = (screenshot->format->Gmask & *buffer) >> 8;
		auto b = screenshot->format->Bmask & *buffer;
		Uint8 avg = (55 * r + 183 * g + 18 * b) >> 8; // BT.709
		*buffer = avg << 16 | avg << 8 | avg | Amask;
	}

	tgt = SDL_CreateTextureFromSurface(renderer, screenshot);
	SDL_FreeSurface(screenshot);
	SDL_SetRenderTarget(renderer, renderTarget);
	return tgt;
}
