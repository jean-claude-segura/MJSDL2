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

#define	DONTUSE		0xFF
#define	EXIT		0xFE
#define	RESTART		0xFD
#define	HINT		0xFC
#define	TURN		0xFB
#define	NORTH		0xFA
#define	SOUTH		0xF9
#define	EAST		0xF8
#define	WEST		0xF7

class GraphicBoard
{
public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();

private:
	void Init();
	void LoadFaceMask();
	void LoadTile(SDL_Texture*& tileSurface, SDL_Texture*& faceSurface, const char* szPath, SDL_Surface* facedown);
	void LoadTile(SDL_Texture*& tileSurface, const char* szPath);
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
	void InterfaceClicked(int index);
	void setClicked(const int x, const int y);
	void ThrowException(const int);
	void LoadUI();
	void LoadButton(SDL_Texture*& button, const std::string& strPath, const std::string& strName);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface
		* virtualmousescreen,
		* mousemask,
		* mousemasktiny;
	SDL_Texture
		* textureBackground,
		* RestartBtn,
		* HintBtn,
		* TurnBtn,
		* EstBtn,
		* SudBtn,
		* OuestBtn,
		* NordBtn,
		* ExitBtn,
		* Inverted,
		* FaceMask;
	int Height, Width;
	SDL_Texture* dominos[42] = { NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL
	};
	SDL_Texture* faces[42] = { NULL, NULL, NULL, NULL, NULL , NULL, NULL,
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
	SDL_Event exitEvent;
};
