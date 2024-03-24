#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>


void SDL_UpperBlitInverted(SDL_Surface* src, SDL_Surface* dest, SDL_Rect& coordonnees);