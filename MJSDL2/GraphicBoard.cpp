#include "GraphicBoard.h"

GraphicBoard::GraphicBoard() : selected(-1), direction(3), Height(0), Width(0)
{
	window = NULL;
	renderer = NULL;
	RestartBtn = NULL;
	HintBtn = NULL;
	TurnBtn = NULL;
	EstBtn = NULL;
	SudBtn = NULL;
	OuestBtn = NULL;
	NordBtn = NULL;
	ExitBtn = NULL;
	exitEvent.type = SDL_QUIT;
	textureBackground = NULL;
	MouseMask = NULL;
	FaceMask = NULL;
	Inverted = NULL;
	textureMouseMap = NULL;
	textureBackgroundVictory = NULL;
	textureGreyedBackground = NULL;
}

GraphicBoard::~GraphicBoard()
{
	// Texture;
	if (MouseMask != NULL)
		SDL_DestroyTexture(MouseMask);
	for (int i = 0; i < 42; ++i)
	{
		if (dominos[i] != NULL)
			SDL_DestroyTexture(dominos[i]);
	}
	for (int i = 0; i < 42; ++i)
	{
		if (faces[i] != NULL)
			SDL_DestroyTexture(faces[i]);
	}
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 42; ++j)
		{
			if (dominoscache[i] != NULL)
				SDL_DestroyTexture(dominoscache[i][j]);
		}

	if (textureMouseMap != NULL)
		SDL_DestroyTexture(textureMouseMap);
	if (FaceMask != NULL)
		SDL_DestroyTexture(FaceMask);
	if (Inverted != NULL)
		SDL_DestroyTexture(Inverted);
	if (RestartBtn != NULL)
		SDL_DestroyTexture(RestartBtn);
	if (HintBtn != NULL)
		SDL_DestroyTexture(HintBtn);
	if (TurnBtn != NULL)
		SDL_DestroyTexture(TurnBtn);
	if (NordBtn != NULL)
		SDL_DestroyTexture(NordBtn);
	if (OuestBtn != NULL)
		SDL_DestroyTexture(OuestBtn);
	if (SudBtn != NULL)
		SDL_DestroyTexture(SudBtn);
	if (EstBtn != NULL)
		SDL_DestroyTexture(EstBtn);
	if (ExitBtn != NULL)
		SDL_DestroyTexture(ExitBtn);
	if (textureBackground != NULL)
		SDL_DestroyTexture(textureBackground);
	if (textureBackgroundVictory != NULL)
		SDL_DestroyTexture(textureBackgroundVictory);
	if (textureGreyedBackground != NULL)
		SDL_DestroyTexture(textureGreyedBackground);

	// Renderer :
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);

	// Windows :
	if (window != NULL)
		SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}

void GraphicBoard::ThrowException(const int i)
{
	throw i;
}

void GraphicBoard::Init()
{
#ifdef _DEBUG	
	//plateau.Test();
	//plateau.TestLocked();
#endif

	SDL_SetMainReady();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << stderr << "could not initialize sdl2 : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2")) {
		std::cout << stderr << "could not initialize render scale quality : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	window = SDL_CreateWindow(
		"Mah Jongg SDL2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		DM.w, DM.h,
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
	);
	if (window == NULL) {
		std::cout << stderr << "could not create window: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		std::cout << stderr << "could not create renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	Width = 3840;
	Height = 2160;
	SDL_RenderSetLogicalSize(renderer, Width, Height);

	textureMouseMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	if (textureMouseMap == NULL)
	{
		std::cout << stderr << "could not create mouse map texture: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	LoadResources();
	LoadUI();

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);

#ifdef _DEBUG
	plateau.InitBoard();
	//plateau.InitBoardLockedHorizontal(2);
#else
	plateau.InitBoard();
#endif
	plateau.SortBoard(direction);
	itNextMove = plateau.GetMovesLeft().begin();
	itPrevMove = plateau.GetMovesLeft().end();

	Refresh(true, true);
}

void GraphicBoard::LoadFaceMask()
{
	auto temp = IMG_Load("./tiles/Blank/facedown.svg");
	auto facedown = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (facedown == NULL) {
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		FaceMask = SDL_CreateTextureFromSurface(renderer, facedown);
		if (FaceMask == NULL)
		{
			SDL_FreeSurface(facedown);
			SDL_FreeSurface(temp);
			std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_FreeSurface(facedown);
		SDL_FreeSurface(temp);
	}
}

void GraphicBoard::LoadMouseMask()
{
	auto temp = IMG_Load("./tiles/Blank/blank.svg");
	auto blank = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (blank == NULL) {
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		MouseMask = SDL_CreateTextureFromSurface(renderer, blank);
		if (MouseMask == NULL)
		{
			SDL_FreeSurface(blank);
			SDL_FreeSurface(temp);
			std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_FreeSurface(blank);
		SDL_FreeSurface(temp);
	}
}

void GraphicBoard::LoadTile(SDL_Texture*& tileTexture, const char* szPath)
{
	if (tileTexture != NULL)
		SDL_DestroyTexture(tileTexture);
	auto temp = IMG_Load(szPath);
	auto tileSurface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	tileTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
	if (tileTexture == NULL)
	{
		SDL_FreeSurface(tileSurface);
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create tile " << szPath << " : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_FreeSurface(tileSurface);
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadTile(const int istart, const int iend, const std::string& path)
{
	std::vector<std::string> vPaths;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!entry.is_directory())
			vPaths.emplace_back(entry.path().string());
	}
	if (vPaths.size() != (iend - istart))
		ThrowException(1);
	std::sort(vPaths.begin(), vPaths.end());
	auto it = vPaths.begin();
	for (int i = istart; i < iend; ++i)
	{
		LoadTile(dominos[i], it->c_str());
		++it;
	}
}

void GraphicBoard::LoadRamdomTileSet(const int istart, const int iend, const std::string& path)
{
	std::vector<std::string> vPaths;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
			vPaths.emplace_back(entry.path().string());
	}
	if (vPaths.empty())
	{
		LoadTile(istart, iend, path);
	}
	else
	{
		std::sort(vPaths.begin(), vPaths.end());
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(0, vPaths.size() - 1);
		LoadTile(istart, iend, vPaths[uniform_dist(e1)]);
	}
}

void GraphicBoard::LoadRandomBackground(const std::string& path)
{
	std::vector<std::string> vPaths;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!entry.is_directory())
			vPaths.emplace_back(entry.path().string());
	}
	std::sort(vPaths.begin(), vPaths.end());
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, vPaths.size() - 1);
	LoadBackground(vPaths[uniform_dist(e1)]);
}

void GraphicBoard::LoadBackground(const std::string& path)
{
	if (textureBackground != NULL)
		SDL_DestroyTexture(textureBackground);
	if (textureGreyedBackground != NULL)
		SDL_DestroyTexture(textureGreyedBackground);

	auto temp = IMG_Load(path.c_str());
	auto background = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (background == NULL) {
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		textureBackground = SDL_CreateTextureFromSurface(renderer, background);
		if (textureBackground == NULL)
		{
			SDL_FreeSurface(background);
			SDL_FreeSurface(temp);
			std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_ResizeTexture(renderer, textureBackground, Width, Height);
		SDL_GreyscaleTexture(renderer, textureBackground, textureGreyedBackground);
		SDL_RenderCopy(renderer, textureGreyedBackground, NULL, NULL);
	}
	SDL_FreeSurface(background);
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadRandomBackgroundVictory(const std::string& path)
{
	std::vector<std::string> vPaths;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!entry.is_directory())
			vPaths.emplace_back(entry.path().string());
	}
	std::sort(vPaths.begin(), vPaths.end());
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, vPaths.size() - 1);
	LoadBackgroundVictory(vPaths[uniform_dist(e1)]);
}

void GraphicBoard::LoadBackgroundVictory(const std::string& path)
{
	if (textureBackgroundVictory != NULL)
		SDL_DestroyTexture(textureBackgroundVictory);

	auto temp = IMG_Load(path.c_str());
	auto background = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (background == NULL) {
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		textureBackgroundVictory = SDL_CreateTextureFromSurface(renderer, background);
		if (textureBackground == NULL)
		{
			SDL_FreeSurface(background);
			SDL_FreeSurface(temp);
			std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_ResizeTexture(renderer, textureBackgroundVictory, Width, Height);
	}
	SDL_FreeSurface(background);
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadTiles()
{
	// Bambous : 9*4
	LoadRamdomTileSet(0, 9, "./tiles/Bambous/");

	// Cercles : 9*4
	LoadRamdomTileSet(9, 18, "./tiles/Cercles/");

	// Caract�res : 9*4
	LoadRamdomTileSet(18, 27, "./tiles/Caracteres/");

	// Vents : 4*4
	LoadRamdomTileSet(27, 31, "./tiles/Vents/");

	// Dragons : 3*4
	LoadRamdomTileSet(31, 34, "./tiles/Dragons/");

	// Saisons : 1 * 4
	LoadRamdomTileSet(34, 38, "./tiles/Saisons/");

	// Fleurs : 1 * 4
	LoadRamdomTileSet(38, 42, "./tiles/Fleurs/");
}

void GraphicBoard::LoadRandomBackground()
{
	LoadRandomBackground("./background/");
}

void GraphicBoard::LoadRandomBackgroundVictory()
{
	LoadRandomBackgroundVictory("./background-victory/");
}

void GraphicBoard::LoadResources()
{
	LoadFaceMask();
	LoadMouseMask();

	LoadTiles();
	LoadRandomBackground();
	LoadRandomBackgroundVictory();
}

void GraphicBoard::LoadButton(SDL_Texture*& button, const std::string& strPath, const std::string& strName)
{
	if (button != NULL)
		SDL_DestroyTexture(button);
	auto temp = IMG_Load(strPath.c_str());
	auto tileSurface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	auto buttonSurface = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (buttonSurface == NULL)
	{
		SDL_FreeSurface(temp);
		SDL_FreeSurface(tileSurface);
		std::cout << stderr << "could not create button surface " << strPath << " : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(tileSurface, NULL, buttonSurface, NULL);
		button = SDL_CreateTextureFromSurface(renderer, buttonSurface);
		if (button == NULL)
		{
			SDL_FreeSurface(temp);
			SDL_FreeSurface(tileSurface);
			SDL_FreeSurface(buttonSurface);
			std::cout << stderr << "could not create button texture " << strPath << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_FreeSurface(buttonSurface);
		SDL_FreeSurface(tileSurface);
		SDL_FreeSurface(temp);
	}
}

void GraphicBoard::LoadUI()
{
	LoadButton(EstBtn, "./interface/MJf1-.svg", "EstBtn");
	LoadButton(SudBtn, "./interface/MJf2-.svg", "SudBtn");
	LoadButton(OuestBtn, "./interface/MJf3-.svg", "OuestBtn");
	LoadButton(NordBtn, "./interface/MJf4-.svg", "NordBtn");
	LoadButton(TurnBtn, "./interface/MJd3rv1-.svg", "TurnBtn");
	LoadButton(HintBtn, "./interface/blank.svg", "HintBtn");
	LoadButton(RestartBtn, "./interface/blank.svg", "RestartBtn");
	LoadButton(ExitBtn, "./interface/blank.svg", "ExitBtn");
}

void GraphicBoard::InterfaceClicked(const int index, const bool right)
{
	// Interface clicked :
	// UI buttons needed...
	switch (index)
	{
	case RESTART:
		if (right)
		{
			if (plateau.Solve())
			{
				for (const auto & move : plateau.GetSolution())
				{
					if (0 <= selected && selected < 143)
						clicked[selected] = false;
					selected = -1;
					Refresh(false);

					clicked[move.first] = true;
					clicked[move.second] = true;

					Refresh(false);

					clicked[move.first] = false;
					clicked[move.second] = false;

					plateau.RemovePairOfTiles(move.first, move.second);

					itNextMove = plateau.GetMovesLeft().begin();
					itPrevMove = plateau.GetMovesLeft().end();

					Refresh(false);
				}

				RefreshMouseMap();
			}
		}
		else
		{
			plateau.InitBoard();
			plateau.SortBoard(direction);
			itNextMove = plateau.GetMovesLeft().begin();
			itPrevMove = plateau.GetMovesLeft().end();

			LoadTiles();
			LoadRandomBackground();

			Refresh(true, true);
		}
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case HINT:
		if (right)
		{
			if (!plateau.GetMovesLeft().empty() && itPrevMove != plateau.GetMovesLeft().end())
			{
				if (plateau.RemovePairOfTiles(itPrevMove->first, itPrevMove->second))
				{
					Refresh(true);
					itNextMove = plateau.GetMovesLeft().begin();
					itPrevMove = plateau.GetMovesLeft().end();
#ifdef _DEBUG
					std::cout << std::dec << plateau.getNumberOfTilesLeft() << " tile" << (plateau.getNumberOfTilesLeft() > 1 ? "s" : "") << " left." << std::endl;
					std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
					std::cout << std::dec << plateau.HowManyMovesLeft() << " move" << (plateau.HowManyMovesLeft() > 1 ? "s" : "") << " left." << std::endl;
					auto it = plateau.GetMovesLeft().begin();
					if (it != plateau.GetMovesLeft().end())
					{
						std::cout << "(" << it->first << ";" << it->second << ")";
						for (++it; it != plateau.GetMovesLeft().end(); ++it)
							std::cout << ", (" << it->first << ";" << it->second << ")";
						std::cout << "." << std::endl;
					}
#endif
				}
			}
		}
		else
		{
			if (itNextMove == plateau.GetMovesLeft().end())
				itNextMove = plateau.GetMovesLeft().begin();
			if (itNextMove != plateau.GetMovesLeft().end())
			{
				if (0 <= selected && selected < 143)
					clicked[selected] = false;
				selected = -1;
				Refresh(false);
				clicked[itNextMove->first] = true;
				clicked[itNextMove->second] = true;
				Refresh(false);
				clicked[itNextMove->first] = false;
				clicked[itNextMove->second] = false;
				itPrevMove = itNextMove;
				++itNextMove;
			}
		}
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case TURN:
		itNextMove = plateau.GetMovesLeft().begin();
		itPrevMove = plateau.GetMovesLeft().end();
		if (right)
			--direction;
		else
			++direction;
		direction %= 4;
		plateau.SortBoard(direction);
		if (!plateau.IsEmpty()) Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case NORTH:
		itNextMove = plateau.GetMovesLeft().begin();
		itPrevMove = plateau.GetMovesLeft().end();
		if (right)
			break;
		if (direction == 3)
			direction = 0;
		else if (direction == 2)
			direction = 1;
		plateau.SortBoard(direction);
		if (!plateau.IsEmpty()) Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case EAST:
		itNextMove = plateau.GetMovesLeft().begin();
		itPrevMove = plateau.GetMovesLeft().end();
		if (right)
			break;
		if (direction == 0)
			direction = 1;
		else if (direction == 3)
			direction = 2;
		plateau.SortBoard(direction);
		if (!plateau.IsEmpty()) Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case SOUTH:
		itNextMove = plateau.GetMovesLeft().begin();
		itPrevMove = plateau.GetMovesLeft().end();
		if (right)
			break;
		if (direction == 0)
			direction = 3;
		else if (direction == 1)
			direction = 2;
		plateau.SortBoard(direction);
		if (!plateau.IsEmpty()) Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case WEST:
		itNextMove = plateau.GetMovesLeft().begin();
		itPrevMove = plateau.GetMovesLeft().end();
		if (right)
			break;
		if (direction == 1)
			direction = 0;
		else if (direction == 2)
			direction = 3;
		plateau.SortBoard(direction);
		if (!plateau.IsEmpty()) Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case EXIT:
		if (right)
		{
#ifdef _DEBUG
			RefreshExample();
#endif
			break;
		}
		SDL_PushEvent(&exitEvent);
		break;
	default:
		break;
	}
#ifdef _DEBUG
	std::cout << std::dec << plateau.getNumberOfTilesLeft() << " tile" << (plateau.getNumberOfTilesLeft() > 1 ? "s" : "") << " left." << std::endl;
	std::cout << std::dec << plateau.HowManyMovesLeft() << " move" << (plateau.HowManyMovesLeft() > 1 ? "s" : "") << " left." << std::endl;
	auto it = plateau.GetMovesLeft().begin();
	if (it != plateau.GetMovesLeft().end())
	{
		std::cout << "(" << it->first << ";" << it->second << ")";
		for (++it; it != plateau.GetMovesLeft().end(); ++it)
			std::cout << ", (" << it->first << ";" << it->second << ")";
		std::cout << "." << std::endl;
	}
#endif
}

void GraphicBoard::setLeftClicked(const int x, const int y)
{
	auto index = SDL_TextureReadPixel(renderer, textureMouseMap, SDL_Point{ x, y }, Width);
	index &= 0xFF;
	if (index != 255) // Not background.
	{
		if (index < 144)
		{
			itNextMove = plateau.GetMovesLeft().begin();
			itPrevMove = plateau.GetMovesLeft().end();
			// Tile clicked :
			if (!plateau.IsLocked() && plateau.getRemovableFromIndex(index))
			{
				if (!clicked[index])
				{
					if (selected == -1)
					{
						selected = index;
						clicked[index] = true;
						Refresh(false);
#ifdef _DEBUG
						std::cout << "Tile " << std::dec << plateau.getDominoFromIndex(index).Rank << ", index 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
#endif					
					}
					else
					{
						clicked[index] = true;
						std::cout << "Tile " << std::dec << plateau.getDominoFromIndex(index).Rank << ", index 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
						if (plateau.RemovePairOfTiles(selected, index))
						{
							int temp = selected;
							selected = -1;
							Refresh(true);
							itNextMove = plateau.GetMovesLeft().begin();
							itPrevMove = plateau.GetMovesLeft().end();
							clicked[temp] = false;
							clicked[index] = false;
#ifdef _DEBUG
							std::cout << std::dec << plateau.getNumberOfTilesLeft() << " tile" << (plateau.getNumberOfTilesLeft() > 1 ? "s" : "") << " left." << std::endl;
							std::cout << std::dec << plateau.HowManyMovesLeft() << " move" << (plateau.HowManyMovesLeft() > 1 ? "s" : "") << " left." << std::endl;
							auto it = plateau.GetMovesLeft().begin();
							if (it != plateau.GetMovesLeft().end())
							{
								std::cout << "(" << it->first << ";" << it->second << ")";
								for (++it; it != plateau.GetMovesLeft().end(); ++it)
									std::cout << ", (" << it->first << ";" << it->second << ")";
								std::cout << "." << std::endl;
							}
#endif
						}
						else
						{
							clicked[index] = false;
#ifdef _DEBUG
							std::cout << "Tile " << std::dec << plateau.getDominoFromIndex(index).Rank << ", index 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
#endif					
						}
					}

				}
				else
				{
					// Unclicked :
					clicked[index] = false;
					selected = -1;
					Refresh(false);
#ifdef _DEBUG
					std::cout << "Tile" << std::dec << plateau.getDominoFromIndex(index).Rank << ", index 0x" << std::hex << index << " (" << std::dec << index << ")" << " unclicked." << std::endl;
#endif					
				}
			}
			else
			{
#ifdef _DEBUG
				std::cout << "Tile " << std::dec << plateau.getDominoFromIndex(index).Rank << ", index 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
#endif					
			}
		}
		else
		{
			// Interface clicked :
			// UI buttons needed...
			InterfaceClicked(index);
		}
	}
	else
	{
		// To cancel current hint.
		if (!plateau.GetMovesLeft().empty() && itPrevMove != plateau.GetMovesLeft().end())
		{
			itNextMove = plateau.GetMovesLeft().begin();
			itPrevMove = plateau.GetMovesLeft().end();
			Refresh(true);
		}
	}
}

bool GraphicBoard::isButtonClicked(const int x, const int y)
{
	auto index = SDL_TextureReadPixel(renderer, textureMouseMap, SDL_Point{ x, y }, Width);
	index &= 0xFF;
	return index != 255; // Background.
}

void GraphicBoard::setRightClicked(const int x, const int y)
{
	auto index = SDL_TextureReadPixel(renderer, textureMouseMap, SDL_Point{ x, y }, Width);
	index &= 0xFF;
	if (143 < index && index < 255) // Background / Tiles
	{
		// Interface clicked :
		// UI buttons needed...
		InterfaceClicked(index, true);
	}
}

inline void GraphicBoard::RenderCopyMouseMap(SDL_Texture* Mask, SDL_Rect coordonnees, Uint32 colour, const double angle, const SDL_RendererFlip flip)
{
	auto renderTarget = SDL_GetRenderTarget(renderer);
	auto SDLRenderer = renderer;
	int w, h;
	SDL_QueryTexture(Mask, NULL, NULL, &w, &h);
	auto tgt = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
	{
		SDL_BlendMode textureBlendMode;
		SDL_GetTextureBlendMode(Mask, &textureBlendMode);
		if (SDL_SetTextureBlendMode(Mask, SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_MINIMUM)) == 0)
		{
			SDL_SetRenderTarget(SDLRenderer, tgt);
			SDL_SetRenderDrawColor(SDLRenderer, 0, 0, colour, 0xFF);
			SDL_RenderClear(SDLRenderer);

			SDL_RenderCopy(SDLRenderer, Mask, NULL, NULL);
			SDL_SetTextureBlendMode(tgt, textureBlendMode);
		}
		SDL_SetTextureBlendMode(Mask, textureBlendMode);
	}
	SDL_SetRenderTarget(renderer, renderTarget);
	SDL_RenderCopyEx(renderer, tgt, NULL, &coordonnees, angle, NULL, flip);
	SDL_DestroyTexture(tgt);
}

void GraphicBoard::RefreshMouseMap()
{
	// Save the current rendering target (will be NULL if it is the current window)
	auto renderTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, textureMouseMap);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	if (!plateau.IsEmpty())
	{
		/**/
		// Placement des dominos :
		SDL_Rect coordonnees;

		SDL_Point size;
		SDL_QueryTexture(MouseMask, NULL, NULL, &size.x, &size.y);

		auto tWidth = (Width - (size.x - 40) * 12) >> 1;
		auto tHeight = (Height - (size.y - 40) >> 3) >> 1;

		for (auto& tile : plateau.getLogicalBoard())
		{
			auto temp = arrIndexToCoord[tile.Index];
			auto x = temp.x;
			auto y = temp.y;
			auto z = temp.z;
			auto domino = tile.TileObject;
			auto index = tile.Index;
			if (direction == 3)
			{
				// Down - Left
				coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
				coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
				coordonnees.w = size.x;
				coordonnees.h = size.y;

				RenderCopyMouseMap(MouseMask, coordonnees, index, 0, SDL_FLIP_NONE);
			}
			else if (direction == 0)
			{
				// Up - Left
				coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
				coordonnees.y = y * (size.y - 40) - z * 40 + tHeight;
				coordonnees.w = size.x;
				coordonnees.h = size.y;

				RenderCopyMouseMap(MouseMask, coordonnees, index, 0, SDL_FLIP_VERTICAL);
			}
			else if (direction == 1)
			{
				// Up - Right
				coordonnees.x = x * (size.x - 40) + z * 40 + tWidth;
				coordonnees.y = y * (size.y - 40) - z * 40 + tHeight;
				coordonnees.w = size.x;
				coordonnees.h = size.y;

				RenderCopyMouseMap(MouseMask, coordonnees, index, 180, SDL_FLIP_NONE);
			}
			else
			{
				// Down - Right
				coordonnees.x = x * (size.x - 40) + z * 40 + tWidth;
				coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
				coordonnees.w = size.x;
				coordonnees.h = size.y;

				RenderCopyMouseMap(MouseMask, coordonnees, index, 0, SDL_FLIP_HORIZONTAL);
			}
		}
	}

	/**/
	// Interface :
	SDL_Rect coordonnees;
	SDL_Point size;

	// Ouest :
	SDL_QueryTexture(RestartBtn, NULL, NULL, &size.x, &size.y);

	coordonnees.w = size.x;
	coordonnees.h = size.y;
	coordonnees.x = 0;
	coordonnees.y = (size.y - 20) * 4;
	RenderCopyMouseMap(OuestBtn, coordonnees, WEST, 0, SDL_FLIP_NONE);
	// Sud :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 5;
	RenderCopyMouseMap(SudBtn, coordonnees, SOUTH, 0, SDL_FLIP_NONE);
	// Turn :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 4;
	RenderCopyMouseMap(TurnBtn, coordonnees, TURN, 0, SDL_FLIP_NONE);
	// Nord :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 3;
	RenderCopyMouseMap(NordBtn, coordonnees, NORTH, 0, SDL_FLIP_NONE);
	// Est :
	coordonnees.x = (size.x << 1) - 40;
	coordonnees.y = (size.y - 20) * 4;
	RenderCopyMouseMap(EstBtn, coordonnees, EAST, 0, SDL_FLIP_NONE);
	// Hint :
	coordonnees.x = size.x - 20;
	coordonnees.y = size.y - 20;
	RenderCopyMouseMap(HintBtn, coordonnees, HINT, 0, SDL_FLIP_NONE);
	// Restart :
	coordonnees.x = size.x - 20;
	coordonnees.y = 0;
	RenderCopyMouseMap(RestartBtn, coordonnees, RESTART, 0, SDL_FLIP_NONE);
	// Exit
	coordonnees.x = Width - size.x;
	coordonnees.y = 0;
	RenderCopyMouseMap(ExitBtn, coordonnees, EXIT, 0, SDL_FLIP_NONE);
	/**/

	// Restore the render target
	SDL_SetRenderTarget(renderer, renderTarget);
}

#ifdef _DEBUG
void GraphicBoard::RefreshExample()
{
	SDL_Texture* screen = NULL;
	auto renderTarget = SDL_GetRenderTarget(renderer);

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, Width, Height);

	SDL_SetRenderTarget(renderer, screen);

	SDL_RenderClear(renderer);

	// Copie du fond :
	SDL_RenderCopy(renderer, textureBackground, NULL, NULL);

	/**/
	SDL_Rect coordonnees;

	if (false)
	{
		// Fin de partie :
		//for (int z = 0; z < 5; ++z)
		{
			int z = 0;
			int domino = 0;
			for (int y = 5; y >= 0; --y)
			{
				for (int x = 0; x < 7; ++x)
				{
					SDL_Point size;
					SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);

					auto tHeight = (Height - (size.y - 40) * 6) / 2;
					auto tWidth = (Width - (size.x - 40) * 7) / 2;

					coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
					coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
					coordonnees.w = size.x;
					coordonnees.h = size.y;

					SDL_RenderCopy(renderer, dominos[domino++], NULL, &coordonnees);
				}
			}
		}

		if (false)
		{
			// Rect parameter
			SDL_Point sizeShift;
			SDL_QueryTexture(MouseMask, NULL, NULL, &sizeShift.x, &sizeShift.y);
			auto tHeight = (Height - (sizeShift.y - 40) * 6) / 2;
			auto tWidth = (Width - (sizeShift.x - 40) * 7) / 2;

			SDL_Rect tgtRect;
			tgtRect.x = tWidth;
			tgtRect.y = tHeight;
			tgtRect.w = 6 * sizeShift.x - 30;
			if (true)
			{
				tgtRect.h = 5 * sizeShift.y + 54;

				if (true)
				{
					// pointer algo :
					SDL_FireOnTextureRectLinear(renderer, renderTarget, screen, &tgtRect, Width >> 2, Height >> 2, 3, 0xC0);
					//SDL_FireOnTextureRectLinear(renderer, renderTargetOrg, screen, &tgtRect, 6 * sizeShift.x - 30, sizeShift.y, 2, 0xC0);
				}
				else if (false)
				{
					// x, y algo :
					SDL_FireOnTextureRect(renderer, renderTarget, screen, &tgtRect, Width >> 2, Height >> 2, 1, 0xC0);
				}
				else
				{
					SDL_FireOnTextureRectLinear(renderer, renderTarget, screen, &tgtRect, Width >> 2, Height >> 2, 0, 0xC0);
				}
			}
			else
			{
				// x, y algo, 6 ranks :
				tgtRect.h = sizeShift.y;

				if (false)
				{
					SDL_FireOnTilesRect(renderer, renderTarget, screen, &tgtRect, 6 * sizeShift.x - 30, sizeShift.y, 0, 0xC0);
				}
				else
				{
					// pointer algo :
					SDL_FireOnTextureRectLinear(renderer, renderTarget, screen, &tgtRect, 6 * sizeShift.x - 30, sizeShift.y, 1, 0xC0);
				}
			}
		}
		else if (false)
		{
			// x, y algo, full screen with texture target parameter
			SDL_FireOnTexture(renderer, renderTarget, Width >> 2, Height >> 2, 0, 0xC0);
		}
		else
		{
			SDL_ExplosionOnTexture(renderer, renderTarget, Width >> 2, Height >> 2, 500, 0xC0);
		}
		SDL_SetRenderTarget(renderer, renderTarget);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
	}
	else if(false)
	{
		// x, y algo, full screen
		SDL_SetRenderTarget(renderer, renderTarget);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
		SDL_FireOnRenderer(renderer, Width >> 2, Height >> 2, 0);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
	}
	else if (false)
	{
		SDL_RenderCopy(renderer, textureBackgroundVictory, NULL, NULL);
		SDL_ExplosionOnTexture(renderer, renderTarget, Width >> 2, Height >> 2, 500, 0xC0);
		SDL_SetRenderTarget(renderer, renderTarget);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
	}
	else if (false)
	{
		SDL_RenderCopy(renderer, textureBackgroundVictory, NULL, NULL);
		SDL_FireworkOnTexture(renderer, renderTarget, Width >> 2, Height >> 2, 500, 0xC0);
		SDL_SetRenderTarget(renderer, renderTarget);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
	}
	else
	{
		SDL_RenderCopy(renderer, textureBackgroundVictory, NULL, NULL);
		SDL_FireworksOnTexture(renderer, renderTarget, Width >> 2, Height >> 2, 500, 0xC0);
		SDL_SetRenderTarget(renderer, renderTarget);
		SDL_RenderCopy(renderer, textureBackground, NULL, NULL);
	}

	SDL_DestroyTexture(screen);
	// Interface :
	// Ouest :
	SDL_Point size;
	SDL_QueryTexture(RestartBtn, NULL, NULL, &size.x, &size.y);

	coordonnees.w = size.x;
	coordonnees.h = size.y;
	coordonnees.x = 0;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, OuestBtn, NULL, &coordonnees);
	// Sud :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 5;
	SDL_RenderCopy(renderer, SudBtn, NULL, &coordonnees);
	// Turn :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, TurnBtn, NULL, &coordonnees);
	// Nord :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 3;
	SDL_RenderCopy(renderer, NordBtn, NULL, &coordonnees);
	// Est :
	coordonnees.x = (size.x << 1) - 40;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, EstBtn, NULL, &coordonnees);
	// Hint :
	coordonnees.x = size.x - 20;
	coordonnees.y = size.y - 20;
	SDL_RenderCopy(renderer, HintBtn, NULL, &coordonnees);
	// Restart :
	coordonnees.x = size.x - 20;
	coordonnees.y = 0;
	SDL_RenderCopy(renderer, RestartBtn, NULL, &coordonnees);
	// Exit
	coordonnees.x = Width - size.x;
	coordonnees.y = 0;
	SDL_RenderCopy(renderer, ExitBtn, NULL, &coordonnees);
	/**/

	//SDL_RenderCopy(renderer, textureMouseMap, NULL, NULL);

	SDL_RenderPresent(renderer);
}
#endif

inline SDL_Texture* GraphicBoard::SetFace(SDL_Texture* texture, SDL_Rect coordonnees, SDL_Texture*& Face)
{
	SDL_Texture* faceMaskCopy = NULL;
	SDL_DuplicateTexture(renderer, FaceMask, faceMaskCopy);

	SDL_CutTextureOnAlpha(renderer, texture, faceMaskCopy);

	SDL_DuplicateTexture(renderer, faceMaskCopy, Face);

	SDL_DestroyTexture(faceMaskCopy);

	return Face;
}

inline void GraphicBoard::RenderCopy(const double x, const double y, const double z, const int domino, const int index, const SDL_Point& org, const SDL_Point& shift, const double angle, const SDL_RendererFlip flip)
{
	if (dominoscache[flip][domino] == NULL)
	{
		Uint32 format;
		int w, h;
		SDL_BlendMode blendmode;

		auto renderTarget = SDL_GetRenderTarget(renderer);

		SDL_QueryTexture(dominos[domino], &format, NULL, &w, &h);
		SDL_GetTextureBlendMode(dominos[domino], &blendmode);

		dominoscache[flip][domino] = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
		SDL_SetTextureBlendMode(dominoscache[flip][domino], SDL_BLENDMODE_NONE);
		SDL_SetRenderTarget(renderer, dominoscache[flip][domino]);

		SDL_Point size;
		SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);
		SDL_Rect coordonnees;
		coordonnees.x = 0;
		coordonnees.y = 0;
		coordonnees.w = size.x;
		coordonnees.h = size.y;

		SDL_RenderCopyEx(renderer, dominos[domino], NULL, &coordonnees, angle, NULL, flip);

		if (faces[domino] == NULL)
			SetFace(dominos[domino], coordonnees, faces[domino]);
		SDL_QueryTexture(faces[domino], NULL, NULL, &size.x, &size.y);
		coordonnees.x = shift.x;
		coordonnees.y = shift.y;
		coordonnees.w = size.x;
		coordonnees.h = size.y;

		SDL_RenderCopy(renderer, faces[domino], NULL, &coordonnees);

		SDL_SetTextureBlendMode(dominoscache[flip][domino], blendmode);
		SDL_SetRenderTarget(renderer, renderTarget);
	}

	SDL_Point size;
	SDL_QueryTexture(dominoscache[flip][domino], NULL, NULL, &size.x, &size.y);
	SDL_Rect coordonnees;
	coordonnees.w = size.x;
	coordonnees.h = size.y;
	coordonnees.x = org.x;
	coordonnees.y = org.y;

	if (clicked[index])
		SDL_RenderCopy(renderer, SDL_InvertTexture(renderer, dominoscache[flip][domino], Inverted), NULL, &coordonnees);
	else
		SDL_RenderCopy(renderer, dominoscache[flip][domino], NULL, &coordonnees);
}

void GraphicBoard::DisplayInterface()
{
	// Interface :
	// Ouest :
	SDL_Point size;
	SDL_QueryTexture(RestartBtn, NULL, NULL, &size.x, &size.y);

	SDL_Rect coordonnees;
	coordonnees.w = size.x;
	coordonnees.h = size.y;
	coordonnees.x = 0;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, OuestBtn, NULL, &coordonnees);
	// Sud :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 5;
	SDL_RenderCopy(renderer, SudBtn, NULL, &coordonnees);
	// Turn :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, TurnBtn, NULL, &coordonnees);
	// Nord :
	coordonnees.x = size.x - 20;
	coordonnees.y = (size.y - 20) * 3;
	SDL_RenderCopy(renderer, NordBtn, NULL, &coordonnees);
	// Est :
	coordonnees.x = (size.x << 1) - 40;
	coordonnees.y = (size.y - 20) * 4;
	SDL_RenderCopy(renderer, EstBtn, NULL, &coordonnees);
	// Hint :
	coordonnees.x = size.x - 20;
	coordonnees.y = size.y - 20;
	SDL_RenderCopy(renderer, HintBtn, NULL, &coordonnees);
	// Restart :
	coordonnees.x = size.x - 20;
	coordonnees.y = 0;
	SDL_RenderCopy(renderer, RestartBtn, NULL, &coordonnees);
	// Exit
	coordonnees.x = Width - size.x;
	coordonnees.y = 0;
	SDL_RenderCopy(renderer, ExitBtn, NULL, &coordonnees);
	/**/
}


void GraphicBoard::Refresh(const bool refreshMouseMap, const bool oneByOne)
{
	if (refreshMouseMap) RefreshMouseMap();

	SDL_Texture* screen = NULL;
	auto renderTargetOrg = SDL_GetRenderTarget(renderer);
	if (plateau.IsLocked())
	{
		screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetRenderTarget(renderer, screen);
	}
	SDL_RenderClear(renderer);

	SDL_Texture* currTextureBackground = NULL;
	// Copie du fond :
	if (plateau.IsLockedFromStart() || plateau.IsLockedFromMove())
	{
		currTextureBackground = textureGreyedBackground;
	}
	else
	{
		currTextureBackground = textureBackground;
	}
	// Copie du fond :
	if (SDL_RenderCopy(renderer, currTextureBackground, NULL, NULL) < 0)
	{
		std::cout << stderr << "could not copy background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	if(oneByOne)
		DisplayInterface();
	if (!plateau.IsEmpty())
	{
		// Placement des dominos :
		SDL_Rect coordonnees;

		SDL_Point sizeMask;
		SDL_QueryTexture(dominos[0], NULL, NULL, &sizeMask.x, &sizeMask.y);
		auto tWidth = (Width - (sizeMask.x - 40) * 12) >> 1;
		auto tHeight = (Height - (sizeMask.y - 40) >> 3) >> 1;

		for (const auto& tile : plateau.getLogicalBoard())
		{
			auto temp = arrIndexToCoord[tile.Index];
			auto x = temp.x;
			auto y = temp.y;
			auto z = temp.z;
			auto domino = tile.TileObject;
			auto index = tile.Index;
			if (direction == 3)
			{
				// Down - Left
				SDL_Point size;
				SDL_QueryTexture(dominos[domino.Rank], NULL, NULL, &size.x, &size.y);
				coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
				coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
				coordonnees.w = size.x;
				coordonnees.h = size.y;

				if (clicked[index])
					SDL_RenderCopy(renderer, SDL_InvertTexture(renderer, dominos[domino.Rank], Inverted), NULL, &coordonnees);
				else
					SDL_RenderCopy(renderer, dominos[domino.Rank], NULL, &coordonnees);
			}
			else if (direction == 0)
			{
				// Up - Left
				SDL_Point org;
				org.x = x * (sizeMask.x - 40) - z * 40 + tWidth;
				org.y = y * (sizeMask.y - 40) - z * 40 + tHeight;
				RenderCopy(x, y, z, domino.Rank, index, org, SDL_Point{ 0, -38 }, 0, SDL_FLIP_VERTICAL);
			}
			else if (direction == 1)
			{
				// Up - Right
				SDL_Point org;
				org.x = x * (sizeMask.x - 40) + z * 40 + tWidth;
				org.y = y * (sizeMask.y - 40) - z * 40 + tHeight;
				RenderCopy(x, y, z, domino.Rank, index, org, SDL_Point{ 33, -38 }, 180, SDL_FLIP_NONE);
			}
			else
			{
				// Down - Right
				SDL_Point org;
				org.x = x * (sizeMask.x - 40) + z * 40 + tWidth;
				org.y = y * (sizeMask.y - 40) + z * 40 + tHeight;
				RenderCopy(x, y, z, domino.Rank, index, org, SDL_Point{ 33, 0 }, 0, SDL_FLIP_HORIZONTAL);
			}

			if (oneByOne)
				SDL_RenderPresent(renderer);
		}
	}

	// Fin de partie :
	if (plateau.IsLocked())
	{
		if (plateau.IsEmpty())
		{
			auto effect = 2;// (int)(3. * (rand() / (RAND_MAX + 1.0)));
			switch (effect)
			{
			default:
				SDL_FireOnTexture(renderer, renderTargetOrg, Width >> 2, Height >> 2, 1, 0xA0);
				break;
			case 1:
				{
					SDL_Rect coordonnees;
					//for (int z = 0; z < 5; ++z)
					{
						int z = 0;
						int domino = 0;
						for (int y = 5; y >= 0; --y)
						{
							for (int x = 0; x < 7; ++x)
							{
								SDL_Point size;
								SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);

								auto tHeight = (Height - (size.y - 40) * 6) / 2;
								auto tWidth = (Width - (size.x - 40) * 7) / 2;

								coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
								coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
								coordonnees.w = size.x;
								coordonnees.h = size.y;

								SDL_RenderCopy(renderer, dominos[domino++], NULL, &coordonnees);
							}
						}
					}
					SDL_Point sizeShift;
					SDL_QueryTexture(MouseMask, NULL, NULL, &sizeShift.x, &sizeShift.y);
					auto tHeight = (Height - (sizeShift.y - 40) * 6) / 2;
					auto tWidth = (Width - (sizeShift.x - 40) * 7) / 2;

					SDL_Rect tgtRect;
					tgtRect.x = tWidth;
					tgtRect.y = tHeight;
					tgtRect.w = 6 * sizeShift.x - 30;
					tgtRect.h = 5 * sizeShift.y + 54;
					SDL_FireOnTextureRect(renderer, renderTargetOrg, screen, &tgtRect, Width >> 2, Height >> 2, 1, 0xA0);
				}
				break;
			case 2:
				SDL_RenderCopy(renderer, textureBackgroundVictory, NULL, NULL);
				//DisplayInterface();
				SDL_FireworksOnTexture(renderer, renderTargetOrg, Width >> 1, Height >> 1, 500, 0xC0);
				break;
			}
			SDL_SetRenderTarget(renderer, renderTargetOrg);
			SDL_RenderCopy(renderer, currTextureBackground, NULL, NULL);
			LoadRandomBackgroundVictory();
		}
		else
		{
			// D�faite :
			SDL_Texture* greyScreen = NULL;
			SDL_GreyscaleTexture(renderer, screen, greyScreen);
			SDL_SetRenderTarget(renderer, renderTargetOrg);
			SDL_RenderCopy(renderer, greyScreen, NULL, NULL);
			SDL_DestroyTexture(greyScreen);
		}
		SDL_DestroyTexture(screen);
	}

#ifdef _DEBUG
	/*SDL_FireOnRenderer(renderer, Width >> 2, Height >> 2, 1);
	SDL_FireOnRenderer(renderer, Width >> 2, Height >> 2, 0);*/
#endif

#ifdef _DEBUG
	//SDL_RenderCopy(renderer, textureMouseMap, NULL, NULL);
#endif
	DisplayInterface();
	SDL_RenderPresent(renderer);
}

void GraphicBoard::WhatsLeft()
{
	SDL_RenderClear(renderer);

	// Copie du fond :
	if (SDL_RenderCopy(renderer, textureBackground, NULL, NULL) < 0)
	{
		std::cout << stderr << "could not copy renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	/**/
	SDL_Point sizeMask;
	SDL_QueryTexture(dominos[0], NULL, NULL, &sizeMask.x, &sizeMask.y);
	auto tWitdh = (Width - (sizeMask.y + 40 * 3)) >> 3;
	SDL_Rect coordonnees;
	int board[8][6];
	for (int x = 0; x < 6; ++x)
		for (int y = 0; y < 8; ++y)
			board[y][x] = 0;

	for (auto& index : plateau.getWhatsLeft())
	{
		int domino = plateau.getDominoFromIndex(index).Rank;
		int x = (domino >> 3);
		int y = (domino & 0b111);
		++board[y][x];
	}

	for (int x = 0; x < 6; ++x)
		for (int y = 7; y >= 0; --y)
		{
			int domino = y + (x << 3);
			int t = board[y][x];
			for (int z = 0; z < board[y][x]; ++z)
			{
				SDL_Point size;
				SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);
				coordonnees.x = x * sizeMask.y + z * 40 + x * tWitdh;
				coordonnees.y = y * (sizeMask.y - 40);
				coordonnees.w = size.x;
				coordonnees.h = size.y;
				SDL_RenderCopy(renderer, dominos[domino], NULL, &coordonnees);
			}
		}
	/**/

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);
}

void GraphicBoard::Loop()
{
	Init();

	SDL_Event event;
	bool done = false;
	while (!done && SDL_WaitEvent(&event)) // SDL_PollEvent(&event) et adieu l'autonomie...
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN:
				break;
			case SDLK_ESCAPE:
				SDL_PushEvent(&exitEvent);
				break;
			case SDLK_F1:
				InterfaceClicked(HINT);
				break;
			case SDLK_F2:
				InterfaceClicked(RESTART);
				break;
			case SDLK_UP:
				InterfaceClicked(NORTH);
				break;
			case SDLK_RIGHT:
				InterfaceClicked(EAST);
				break;
			case SDLK_DOWN:
				InterfaceClicked(SOUTH);
				break;
			case SDLK_LEFT:
				InterfaceClicked(WEST);
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP));
				{
#ifdef _DEBUG
					std::cout << "(" << event.button.x << ";" << event.button.y << ") left clicked." << std::endl;
#endif
					setLeftClicked(event.button.x, event.button.y);
				}
				break;
			case SDL_BUTTON_RIGHT:
				if (!isButtonClicked(event.button.x, event.button.y))
				{
					itNextMove = plateau.GetMovesLeft().begin();
					itPrevMove = plateau.GetMovesLeft().end();
#ifdef _DEBUG
					std::cout << "(" << event.button.x << ";" << event.button.y << ") right clicked." << std::endl;
#endif
					if (!plateau.IsEmpty())
					{
						WhatsLeft();
						while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP));
						Refresh(false);
					}
				}
				else
				{
					auto index = (SDL_TextureReadPixel(renderer, textureMouseMap, SDL_Point{ event.button.x, event.button.y }, Width) & 0xFF);
					if (index < 144)
					{
						itNextMove = plateau.GetMovesLeft().begin();
						itPrevMove = plateau.GetMovesLeft().end();

						if (!plateau.IsLocked() && plateau.getRemovableFromIndex(index))
						{
							if (0 <= selected && selected < 143)
								clicked[selected] = false;
							selected = -1;
							auto autoselected = plateau.getDominoFromIndex(index);
							std::vector<int> relevantTiles;
							for (const auto& tile : plateau.getLogicalBoard())
							{
								auto temp = arrIndexToCoord[tile.Index];
								auto x = temp.x;
								auto y = temp.y;
								auto z = temp.z;
								auto domino = tile.TileObject;
								auto index = tile.Index;

								if (domino.Pairing == autoselected.Pairing &&
									plateau.getRemovableFromIndex(index))
								{
									clicked[index] = true;
									relevantTiles.emplace_back(index);
								}
							}
							Refresh(false);
							while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP));
							if (event.button.button == SDL_BUTTON_LEFT && (relevantTiles.size() == 2 || relevantTiles.size() == 4))
							{
								for (const auto& index : relevantTiles)
									clicked[index] = false;
								bool result = plateau.RemovePairOfTiles(relevantTiles[0], relevantTiles[1]);
								if (relevantTiles.size() == 4)
									result |= plateau.RemovePairOfTiles(relevantTiles[2], relevantTiles[3]);
								if (result)
								{
									Refresh(true);
									itNextMove = plateau.GetMovesLeft().begin();
									itPrevMove = plateau.GetMovesLeft().end();
#ifdef _DEBUG
									std::cout << std::dec << plateau.getNumberOfTilesLeft() << " tile" << (plateau.getNumberOfTilesLeft() > 1 ? "s" : "") << " left." << std::endl;
									std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
									std::cout << std::dec << plateau.HowManyMovesLeft() << " move" << (plateau.HowManyMovesLeft() > 1 ? "s" : "") << " left." << std::endl;
									auto it = plateau.GetMovesLeft().begin();
									if (it != plateau.GetMovesLeft().end())
									{
										std::cout << "(" << it->first << ";" << it->second << ")";
										for (++it; it != plateau.GetMovesLeft().end(); ++it)
											std::cout << ", (" << it->first << ";" << it->second << ")";
										std::cout << "." << std::endl;
									}
#endif
								}
							}
							else
							{
								for (const auto& index : relevantTiles)
									clicked[index] = false;
								Refresh(false);
							}
						}
					}
					else
					{
						while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP));
#ifdef _DEBUG
						std::cout << "(" << event.button.x << ";" << event.button.y << ") right clicked." << std::endl;
#endif
						setRightClicked(event.button.x, event.button.y);
					}
				}
				break;
			default:
				break;
			}
			break;
			// Ev�nement d�clench� par une fermeture de la fen�tre:
		case SDL_QUIT:
			done = true;
			break;
		default:
			break;
		}
	}
}

