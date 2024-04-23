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
#include "Tools.h"

// https://github.com/libsdl-org/SDL_ttf/tree/main/external
// https://stackoverflow.com/questions/4998974/visual-studio-2008-moving-files-at-build-to-bin

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
private:
	inline SDL_Texture* SetFace(SDL_Texture* texture, const SDL_Rect coordonnees, SDL_Texture*& Face);
	inline void RenderCopy(const double x, const double y, const double z, const int domino, const int index, const SDL_Point& org, const SDL_Point& shift, const double angle, const SDL_RendererFlip flip);
	inline void RenderCopyMouseMap(SDL_Texture* Mask, SDL_Rect coordonnees, Uint32 colour, const double angle, const SDL_RendererFlip flip);

public:
	GraphicBoard();
	~GraphicBoard();
	void Loop();

private:
	void Init();
	void LoadFaceMask();
	void LoadMouseMask();
	void LoadTile(SDL_Texture*& tileSurface, const char* szPath);
	void LoadTile(const int istart, const int iend, const std::string& path);
	void LoadRamdomTileSet(const int istart, const int iend, const std::string& path);
	void LoadTiles();
	void LoadRandomBackground(const std::string& path);
	void LoadBackground(const std::string& path);
	void LoadRandomBackgroundVictory(const std::string& path);
	void LoadBackgroundVictory(const std::string& path);
	void LoadResources();
	void Refresh(const bool refreshMouseMap, const bool oneByOne = false);
	void DisplayInterface();
#ifdef _DEBUG
	void RefreshExample();
#endif
	void RefreshMouseMap();
	void WhatsLeft();
	void InterfaceClicked(const int index, const bool right = false);
	void setLeftClicked(const int x, const int y);
	bool isButtonClicked(const int x, const int y);
	void setRightClicked(const int x, const int y);
	void ThrowException(const int);
	void LoadUI();
	void LoadButton(SDL_Texture*& button, const std::string& strPath, const std::string& strName);

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture
		* textureBackground,
		* textureBackgroundVictory,
		* textureGreyedBackground,
		* RestartBtn,
		* HintBtn,
		* TurnBtn,
		* EstBtn,
		* SudBtn,
		* OuestBtn,
		* NordBtn,
		* ExitBtn,
		* Inverted,
		* FaceMask,
		* textureMouseMap,
		* MouseMask;
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
	SDL_Texture* dominoscache[3][42] = {
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
		NULL, NULL, NULL, NULL, NULL , NULL, NULL,
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
	int selected;
	std::vector<std::pair<int, int>>::const_iterator itNextMove;
	std::vector<std::pair<int, int>>::const_iterator itPrevMove;
	uint8_t direction;
	SDL_Event exitEvent;
};
