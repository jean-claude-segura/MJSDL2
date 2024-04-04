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

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface
		*virtualscreen,
		*virtualmousescreen,
		*mousescreen,
		*tampon,
		*background,
		*restart,
		*hint,
		*turn,
		*bordermask,
		*facedown,
		*Est,
		*Sud,
		*Ouest,
		*Nord;
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
	int selected;
	std::vector<std::pair<int, int>>::const_iterator itNextMove;
	uint8_t direction;
	SDL_Event exit;
};
