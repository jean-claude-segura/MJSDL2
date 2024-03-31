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

#define	RESTART		0xFE
#define	HINT		0xFD

class GraphicBoard
{
public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();

private:
	void LoadTile(const int i, const std::string& path);
	void LoadBackground(const std::string& path);
	void FreeResources();
	void LoadResources();
	void Refresh();
	void RefreshMouseMap();
	void WhatsLeft();
	void setClicked(const int x, const int y);
	void ThrowException(const int);
	void LoadUI();

	SDL_Window* window = NULL;
	SDL_Surface* virtualscreen = NULL;
	SDL_Surface* virtualmousescreen = NULL;
	SDL_Surface* mousescreen = NULL;
	SDL_Surface* tampon = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* background = NULL;
	SDL_Surface* restart = NULL;
	SDL_Surface* hint = NULL;
	//SDL_Surface* couches[5] = { NULL, NULL, NULL, NULL, NULL };
	int Height, Width;
	SDL_DisplayMode displayCapabilities;
	SDL_Surface* dominos[42] = { NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL		
	};
	Board plateau;
	bool clicked[144] = {
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false
	};
	SDL_Rect ScreenRect;
	int selected = -1;
	std::vector<std::pair<int, int>>::const_iterator itNextMove;
};
