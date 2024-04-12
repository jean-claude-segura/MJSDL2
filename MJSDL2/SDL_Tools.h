#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Tools.h"

void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitXored(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitNegate(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_SetColourOnOpaque(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees, Uint32 ColourToFill);
void SDL_SetGreyScale(SDL_Surface* src);
void SDL_VerticalFlip(SDL_Surface* surface);
void SDL_HorizontalFlip(SDL_Surface* surface);
void SDL_UpperBlitCut(SDL_Surface* src, SDL_Surface* dest);
void SDL_FireOnRenderer(SDL_Renderer* renderer, int Width, int Height, int FireType = 0);
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, int Width, int Height, int FireType = 0, Uint32 Alpha = 0x00);
void SDL_FireOnTexture(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, int Width, int Height, int FireType = 0, Uint32 Alpha = 0x00);
void SDL_FireOnTextureRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, int Width, int Height, int FireType, Uint32 Alpha);
void SDL_FireOnTextureRectLinear(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, int Width, int Height, int FireType, Uint32 Alpha);
void SDL_FireOnTextureBisRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, int Width, int Height, int FireType, Uint32 Alpha);
void SDL_FireOnTilesRect(SDL_Renderer* renderer, SDL_Texture* renderTarget, SDL_Texture* screen, SDL_Rect* tgtRect, int Width, int Height, int FireType, Uint32 Alpha);

/* *********************************************************************************** /
/*                                       INLINE                                       */
/* *********************************************************************************** /

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

inline SDL_Texture* SDL_CutTextureOnAlpha(SDL_Renderer* renderer, SDL_Texture* src, SDL_Texture*& tgt)
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

inline Uint32 SDL_TextureReadPixel(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Point p, int WIDTH)
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
