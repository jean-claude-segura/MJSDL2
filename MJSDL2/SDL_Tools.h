#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>


void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitXored(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_UpperBlitNegate(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);
void SDL_SetColourOnOpaque(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees, Uint32 ColourToFill);
void SDL_SetGreyScale(SDL_Surface* src);
void SDL_VerticalFlip(SDL_Surface* surface);
void SDL_HorizontalFlip(SDL_Surface* surface);
void SDL_UpperBlitCut(SDL_Surface* src, SDL_Surface* dest);

/* *************************************************** /
/*                       INLINE                       */
/* *************************************************** /
/*
* https://discourse.libsdl.org/t/sdl-composecustomblendmode-error-in-windows/35241/1
*/
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

/*
* https://stackoverflow.com/questions/75873908/how-to-copy-a-texture-to-another-texture-without-pointing-to-the-same-texture
*/
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

inline Uint32 SDL_TextureReadPixel(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Point p, int WIDTH)
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
