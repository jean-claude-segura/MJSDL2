#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Board.h"
#include <vector>
#include <random>
#include "SDL_Tools.h"

// https://fr.vecteezy.com/photo/2680573-texture-bois-fond-texture-bois
// https://github.com/libsdl-org/SDL_ttf/tree/main/external

class GraphicBoard
{
public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();

private:
	void LoadResources();
	void Refresh();
	void WhatsLeft();
	void setClicked(int x, int y);

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* background = NULL;
	SDL_Surface* couches[5] = { NULL, NULL, NULL, NULL, NULL };
	int Height, Width;
	SDL_DisplayMode displayCapabilities;
	SDL_Surface* dominos[42];
	Board plateau;
	std::tuple<double, double, bool> clicked[2] = { {0,0,false}, {0,0,false} };
};
