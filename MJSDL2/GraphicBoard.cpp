#include "GraphicBoard.h"

GraphicBoard::GraphicBoard() : selected(-1), direction(3), Height (0), Width(0)
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
	virtualmousescreen = NULL;
	mousemask = NULL;
	FaceMask = NULL;
	mousemasktiny = NULL;
	Inverted = NULL;
}

GraphicBoard::~GraphicBoard()
{
	// Surfaces :
	if (virtualmousescreen != NULL)
		SDL_FreeSurface(virtualmousescreen);
	if (mousemasktiny != NULL)
		SDL_FreeSurface(mousemasktiny);
	if (mousemask != NULL)
		SDL_FreeSurface(mousemask);

	// Texture;
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

	virtualmousescreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (virtualmousescreen == NULL)
	{
		std::cout << stderr << "could not create virtual mouse screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	LoadResources();
	LoadUI();

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);

	plateau.InitBoard();
	plateau.SortBoard(direction);
	itNextMove = plateau.GetMovesLeft().begin();
	Refresh(true);
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

	// Caractères : 9*4
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

void GraphicBoard::LoadResources()
{
	auto temp = IMG_Load("./tiles/Blank/blank.svg");
	mousemask = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (mousemask == NULL)
	{
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create mouse mask: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	mousemasktiny = SDL_CreateRGBSurface(0, mousemask->w >> 1, mousemask->h >> 1, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (mousemasktiny == NULL)
	{
		SDL_FreeSurface(temp);
		std::cout << stderr << "could not create mouse mask: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_UpperBlitScaled(mousemask, NULL, mousemasktiny, NULL);
	SDL_FreeSurface(temp);

	LoadFaceMask();

	LoadTiles();
	//LoadBackground("./background/10013168.jpg");
	//LoadBackground("./background/vecteezy_wood-texture-background-wood-pattern-texture_2680573.jpg");
	LoadRandomBackground("./background/");
}

void GraphicBoard::LoadButton(SDL_Texture* &button, const std::string &strPath, const std::string& strName)
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

void GraphicBoard::InterfaceClicked(int index)
{
	// Interface clicked :
	// UI buttons needed...
	switch (index)
	{
	case RESTART:
		plateau.InitBoard();
		plateau.SortBoard(direction);
		itNextMove = plateau.GetMovesLeft().begin();
		LoadTiles();
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case HINT:
		if (itNextMove == plateau.GetMovesLeft().end())
			itNextMove = plateau.GetMovesLeft().begin();
		if (itNextMove != plateau.GetMovesLeft().end())
		{
			clicked[selected] = false;
			selected = -1;
			clicked[itNextMove->first] = true;
			clicked[itNextMove->second] = true;
			Refresh(false);
			SDL_Delay(400);
			clicked[itNextMove->first] = false;
			clicked[itNextMove->second] = false;
			Refresh(false);
			++itNextMove;
		}
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case TURN:
		++direction;
		direction %= 4;
		plateau.SortBoard(direction);
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case NORTH:
		if (direction == 3)
			direction = 0;
		else if (direction == 2)
			direction = 1;
		plateau.SortBoard(direction);
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case EAST:
		if (direction == 0)
			direction = 1;
		else if (direction == 3)
			direction = 2;
		plateau.SortBoard(direction);
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case SOUTH:
		if (direction == 0)
			direction = 3;
		else if (direction == 1)
			direction = 2;
		plateau.SortBoard(direction);
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case WEST:
		if (direction == 1)
			direction = 0;
		else if (direction == 2)
			direction = 3;
		plateau.SortBoard(direction);
		Refresh(true);
		SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
		break;
	case EXIT:
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

void GraphicBoard::setClicked(const int x, const int y)
{
	Uint32* pixel = (Uint32*)virtualmousescreen->pixels + (x + y * virtualmousescreen->w );
	Uint32 index = (*pixel) & 0xFF;
	if (index != 255) // Not background.
	{
		if (index < 144)
		{
			// Tile clicked :
			if (!plateau.IsBlocked() && plateau.getRemovableFromIndex(index))
			{
				if (!clicked[index])
				{
					if (selected == -1)
					{
						selected = index;
						clicked[index] = true;
						Refresh(false);
#ifdef _DEBUG
						std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
#endif					
					}
					else
					{
						clicked[index] = true;
						if (plateau.RemovePairOfTiles(selected, index))
						{
							int temp = selected;
							selected = -1;
							Refresh(true);
							itNextMove = plateau.GetMovesLeft().begin();
							clicked[temp] = false;
							clicked[index] = false;
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
						else
						{
							clicked[index] = false;
#ifdef _DEBUG
							std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
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
					std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
#endif					
				}
			}
			else
			{
#ifdef _DEBUG
				std::cout << "Tile 0x" << std::hex << index << " (" << std::dec << index << ")" << " clicked." << std::endl;
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
}

void GraphicBoard::RefreshMouseMap()
{
	// Copie du fond :
	SDL_FillRect(virtualmousescreen, NULL, SDL_MapRGB(virtualmousescreen->format, 0xFF, 0xFF, 0xFF));

	/**/
	// Placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (mousemask->w - 40) * 12) >> 1;
	auto tHeight = (Height - (mousemask->h - 40) >> 3) >> 1;

	for (auto& tile : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(tile);
		auto y = std::get<1>(tile);
		auto z = std::get<2>(tile);
		auto domino = std::get<3>(tile);
		auto index = std::get<4>(tile);
		if (direction == 3)
		{
			coordonnees.x = x * (mousemask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (mousemask->h - 40) + z * 40 + tHeight;
			coordonnees.w = mousemask->w;
			coordonnees.h = mousemask->h;
			SDL_SetColourOnOpaque(mousemask, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, index));
		}
		else if (direction == 0)
		{
			coordonnees.x = x * (mousemask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (mousemask->h - 40) - z * 40 + tHeight;
			coordonnees.w = mousemask->w;
			coordonnees.h = mousemask->h;
			auto temp = SDL_CreateRGBSurface(0, mousemask->w, mousemask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			// Récupération du domino :
			SDL_UpperBlit(mousemask, NULL, temp, NULL);
			SDL_VerticalFlip(temp);

			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, index));

			SDL_FreeSurface(temp);
		}
		else if (direction == 1)
		{
			coordonnees.x = x * (mousemask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (mousemask->h - 40) - z * 40 + tHeight;
			coordonnees.w = mousemask->w;
			coordonnees.h = mousemask->h;
			// Up - Right
			auto temp = SDL_CreateRGBSurface(0, mousemask->w, mousemask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(mousemask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);

			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, index));

			SDL_FreeSurface(temp);
		}
		else
		{
			// Down - Right
			coordonnees.x = x * (mousemask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (mousemask->h - 40) + z * 40 + tHeight;
			coordonnees.w = mousemask->w;
			coordonnees.h = mousemask->h;
			auto temp = SDL_CreateRGBSurface(0, mousemask->w, mousemask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(mousemask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);
			SDL_VerticalFlip(temp);

			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, index));

			SDL_FreeSurface(temp);
		}
	}
	/**/
	SDL_Point size{ mousemasktiny->w, mousemasktiny->h };
	
	coordonnees.w = size.x;
	coordonnees.h = size.y;
	// Ouest :
	coordonnees.x = 0;
	coordonnees.y = (size.y - 20) * 4;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, WEST));
	// Sud :
	coordonnees.x = mousemasktiny->w - 20;
	coordonnees.y = (size.y - 20) * 5;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, SOUTH));
	// Turn :
	coordonnees.x = mousemasktiny->w - 20;
	coordonnees.y = (size.y - 20) * 4;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, TURN));
	// Nord :
	coordonnees.x = mousemasktiny->w - 20;
	coordonnees.y = (size.y - 20) * 3;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, NORTH));
	// Est :
	coordonnees.x = (mousemasktiny->w << 1) - 40;
	coordonnees.y = (size.y - 20) * 4;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, EAST));
	// Hint :
	coordonnees.x = mousemasktiny->w - 20;
	coordonnees.y = size.y - 20;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, HINT));
	// Restart :
	coordonnees.x = mousemasktiny->w - 20;
	coordonnees.y = 0;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, RESTART));
	// Exit
	coordonnees.x = virtualmousescreen->w - mousemasktiny->w;
	coordonnees.y = 0;
	coordonnees.w = mousemasktiny->w;
	coordonnees.h = size.y;
	SDL_SetColourOnOpaque(mousemasktiny, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, 0x00, 0x00, EXIT));
	/**/
}

#ifdef _DEBUG
void GraphicBoard::RefreshExample()
{
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
	Uint32 format;
	int w, h;
	SDL_BlendMode blendmode;

	auto renderTarget = SDL_GetRenderTarget(renderer);

	SDL_QueryTexture(dominos[domino], &format, NULL, &w, &h);
	SDL_GetTextureBlendMode(dominos[domino], &blendmode);

	auto tgt = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
	SDL_SetTextureBlendMode(tgt, SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(renderer, tgt);

	SDL_Point size;
	SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);
	SDL_Rect coordonnees;
	coordonnees.x = 0;
	coordonnees.y = 0;
	coordonnees.w = size.x;
	coordonnees.h = size.y;

	SDL_RenderCopyEx(renderer, dominos[domino], NULL, &coordonnees, angle, NULL, flip);

	SDL_Rect coord;
	if (faces[domino] == NULL)
		SetFace(dominos[domino], coordonnees, faces[domino]);
	SDL_QueryTexture(faces[domino], NULL, NULL, &size.x, &size.y);
	coordonnees.x = 0;
	coordonnees.y = 0;
	coordonnees.x += shift.x;
	coordonnees.y += shift.y;
	coordonnees.w = size.x;
	coordonnees.h = size.y;

	SDL_RenderCopy(renderer, faces[domino], NULL, &coordonnees);

	SDL_SetTextureBlendMode(tgt, blendmode);
	SDL_SetRenderTarget(renderer, renderTarget);

	coordonnees.x = org.x;
	coordonnees.y = org.y;

	if (clicked[index])
		SDL_RenderCopy(renderer, SDL_InvertTexture(renderer, tgt, Inverted), NULL, &coordonnees);
	else
		SDL_RenderCopy(renderer, tgt, NULL, &coordonnees);

	SDL_DestroyTexture(tgt);
}

void GraphicBoard::Refresh(bool refreshMouseMap)
{
	if (refreshMouseMap) RefreshMouseMap();
	SDL_RenderClear(renderer);

	// Copie du fond :
	if (SDL_RenderCopy(renderer, textureBackground, NULL, NULL) < 0)
	{
		std::cout << stderr << "could not copy renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	// Placement des dominos :
	SDL_Rect coordonnees;

	SDL_Point sizeMask;
	SDL_QueryTexture(dominos[0], NULL, NULL, &sizeMask.x, &sizeMask.y);
	auto tWidth = (Width - (sizeMask.x - 40) * 12) >> 1;
	auto tHeight = (Height - (sizeMask.y - 40) >> 3) >> 1;

	for (const auto& tile : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(tile);
		auto y = std::get<1>(tile);
		auto z = std::get<2>(tile);
		auto domino = std::get<3>(tile);
		auto index = std::get<4>(tile);
		if (direction == 3)
		{
			// Down - Left
			SDL_Point size;
			SDL_QueryTexture(dominos[domino], NULL, NULL, &size.x, &size.y);
			coordonnees.x = x * (size.x - 40) - z * 40 + tWidth;
			coordonnees.y = y * (size.y - 40) + z * 40 + tHeight;
			coordonnees.w = size.x;
			coordonnees.h = size.y;

			if (clicked[index])
				SDL_RenderCopy(renderer, SDL_InvertTexture(renderer, dominos[domino], Inverted), NULL, &coordonnees);
			else
				SDL_RenderCopy(renderer, dominos[domino], NULL, &coordonnees);
		}
		else if (direction == 0)
		{
			// Up - Left
			SDL_Point org;
			org.x = x * (sizeMask.x - 40) - z * 40 + tWidth;
			org.y = y * (sizeMask.y - 40) - z * 40 + tHeight;
			RenderCopy(x, y, z, domino, index, org, SDL_Point{0, -38}, 0, SDL_FLIP_VERTICAL);
		}
		else if (direction == 1)
		{
			// Up - Right
			SDL_Point org;
			org.x = x * (sizeMask.x - 40) + z * 40 + tWidth;
			org.y = y * (sizeMask.y - 40) - z * 40 + tHeight;
			RenderCopy(x, y, z, domino, index, org, SDL_Point{ 33, -38 }, 180, SDL_FLIP_NONE);
		}
		else
		{
			// Down - Right
			SDL_Point org;
			org.x = x * (sizeMask.x - 40) + z * 40 + tWidth;
			org.y = y * (sizeMask.y - 40) + z * 40 + tHeight;
			RenderCopy(x, y, z, domino, index, org, SDL_Point{ 33, 0 }, 0, SDL_FLIP_HORIZONTAL);
		}
	}

	// Défaite :
	//SDL_SetTextureColorMod
	/*
		if(false)
		{
			Uint8 r, g, b;
			SDL_GetTextureColorMod(texture, &r, &g, &b);
			SDL_SetTextureColorMod(texture, 255 >> 1, 255 >> 1, 255 >> 1);
			SDL_RenderCopyEx(renderer, texture, NULL, &coordonnees, angle, NULL, flip);
			SDL_SetTextureColorMod(texture, r, g, b);
		}
	*/
	if (plateau.IsBlocked())
	{
		if (plateau.IsEmpty())
		{
			/*
			auto success = SDL_CreateRGBSurface(0, virtualscreen->w, virtualscreen->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			if (success == NULL)
			{
				std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
				ThrowException(1);
			}
			else
			{
				SDL_FillRect(success, NULL, SDL_MapRGBA(success->format, 0x00, 0xFF, 0x00, 0xA0));
				SDL_UpperBlit(success, NULL, virtualscreen, NULL);
				SDL_FreeSurface(success);
			}
			/**/
		}
		else
		{
			/*
			SDL_SetGreyScale(virtualscreen);
			/**/
		}
	}
	
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


//#define _SEEMOUSEMAP
#ifdef _SEEMOUSEMAP
	//SDL_BlitScaled(virtualmousescreen, NULL, tampon, NULL);
	//SDL_BlitScaled(mousescreen, NULL, tampon, NULL);

	auto texture = SDL_CreateTextureFromSurface(renderer, virtualmousescreen);
	if (texture == NULL)
	{
		std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0)
	{
		std::cout << stderr << "could not copy renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	SDL_DestroyTexture(texture);
#endif


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
		int domino = plateau.getDominoFromIndex(index);
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
					//setClicked(event.motion.x, event.motion.y);
					std::cout << "(" << event.button.x << ";" << event.button.y << ") clicked." << std::endl;
					setClicked(event.button.x, event.button.y);
					//plateau.InitBoard();
					//Refresh();
					/*switch (event.type)
					{
					case SDL_MOUSEMOTION:
						break;
					default:
						break;
					}*/
				}
				break;
			case SDL_BUTTON_RIGHT:
				WhatsLeft();
				while (SDL_WaitEvent(&event) && (event.type != SDL_MOUSEBUTTONUP));
				//plateau.InitBoard();
				Refresh(false);
				break;
			default:
				break;
			}
			break;
			// Evénement déclenché par une fermeture de la fenêtre:
		case SDL_QUIT:
			done = true;
			break;
		default:
			break;
		}
	}
}

