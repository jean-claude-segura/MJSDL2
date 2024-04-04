#pragma once
#define SDL_MAIN_HANDLED
//#include <SDL2/SDL.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Board.h"
#include <vector>
#include <random>
#include <filesystem>
#include "SDL_Tools.h"

// https://fr.vecteezy.com/photo/2680573-texture-bois-fond-texture-bois
// https://github.com/libsdl-org/SDL_ttf/tree/main/external

#define	RESTART		0xFE
#define	HINT		0xFD
#define	TURN		0xFC
#define	NORTH		0xFB
#define	SOUTH		0xFA
#define	EAST		0xF9
#define	WEST		0xF8

class GraphicBoard
{
public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();

private:
	void Init();
	void LoadTile(SDL_Surface* &tileSurface, const char* szPath);
	void LoadTile(const int istart, const int iend, const std::string& path);
	void LoadRamdomTileSet(const int istart, const int iend, const std::string& path);
	void LoadTiles();
	void LoadRandomBackground(const std::string& path);
	void LoadBackground(const std::string& path);
	void LoadResources();
	void Refresh(bool);
#ifdef _DEBUG
	void RefreshExample();
#endif
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
	SDL_Surface* turn = NULL;
	SDL_Surface* bordermask = NULL;
	SDL_Surface* facedown = NULL;
	SDL_Surface* Est = NULL;
	SDL_Surface* Sud = NULL;
	SDL_Surface* Ouest = NULL;
	SDL_Surface* Nord = NULL;
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
	uint8_t direction = 3;
};
