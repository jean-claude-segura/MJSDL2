#include "GraphicBoard.h"

GraphicBoard::GraphicBoard()
{
	bInitDone = false;
}

GraphicBoard::~GraphicBoard()
{
	if (tampon != NULL)
		SDL_FreeSurface(tampon);
	if (virtualscreen != NULL)
		SDL_FreeSurface(virtualscreen);
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);
	for (int i = 0; i < 42; ++i)
	{
		if (dominos[i] != NULL)
			SDL_FreeSurface(dominos[i]);
	}
	if (background != NULL)
		SDL_FreeSurface(background);
	if (restart != NULL)
		SDL_FreeSurface(restart);
	if (hint != NULL)
		SDL_FreeSurface(hint);
	if (turn != NULL)
		SDL_FreeSurface(turn);
	if (bordermask != NULL)
		SDL_FreeSurface(bordermask);
	if (facedown != NULL)
		SDL_FreeSurface(facedown);
	if (Nord != NULL)
		SDL_FreeSurface(Nord);
	if (Ouest != NULL)
		SDL_FreeSurface(Ouest);
	if (Sud != NULL)
		SDL_FreeSurface(Sud);
	if (Est != NULL)
		SDL_FreeSurface(Est);
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void GraphicBoard::ThrowException(const int i)
{
	throw i;
}

void GraphicBoard::Init()
{
	plateau.InitFirst();
	SDL_SetMainReady();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << stderr << "could not initialize sdl2 : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	Width = 3840;
	Height = 2160;

	//SDL_GetDisplayMode(0, 0, &displayCapabilities);
	ScreenRect.h = DM.h;
	ScreenRect.w = DM.w;
	ScreenRect.x = 0;
	ScreenRect.y = 0;
	window = SDL_CreateWindow(
		"Mah Jongg SDL2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		DM.w, DM.h,
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL
		SDL_WINDOW_SHOWN
	);
	if (window == NULL) {
		std::cout << stderr << "could not create window: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		std::cout << stderr << "could not create renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	virtualscreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (virtualscreen == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	virtualmousescreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (virtualmousescreen == NULL)
	{
		std::cout << stderr << "could not create virtual mouse screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (tampon == NULL)
	{
		std::cout << stderr << "could not create buffer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	mousescreen = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	if (mousescreen == NULL)
	{
		std::cout << stderr << "could not create mouse screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	LoadResources();
	LoadUI();

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);

	itNextMove = plateau.GetMovesLeft().begin();
	plateau.SortBoard(direction);
	Refresh(true);
	bInitDone = true;
}

void GraphicBoard::LoadTile(SDL_Surface* &tileSurface, const char * szPath)
{
	if (tileSurface != NULL)
		SDL_FreeSurface(tileSurface);
	auto temp = IMG_Load(szPath);
	tileSurface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (tileSurface == NULL)
	{
		std::cout << stderr << "could not create tile " << szPath << " : " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadTile(const int istart, const int iend, const std::string & path)
{
	std::vector<std::string> vPaths;
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if(!entry.is_directory())
			vPaths.emplace_back(entry.path().string());
	}
	if(vPaths.size() != (iend - istart))
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
	background = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (background == NULL) {
		std::cout << stderr << "could not create background: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
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
	LoadTiles();
	//LoadBackground("./background/10013168.jpg");
	//LoadBackground("./background/vecteezy_wood-texture-background-wood-pattern-texture_2680573.jpg");
	LoadRandomBackground("./background/");

	auto temp = IMG_Load("./tiles/Blank/blank.svg");
	bordermask = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (bordermask == NULL)
	{
		std::cout << stderr << "could not create tile mask: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_FreeSurface(temp);

	temp = IMG_Load("./tiles/Blank/facedown.svg");
	facedown = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (facedown == NULL)
	{
		std::cout << stderr << "could not create tile mask: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadUI()
{
	SDL_Surface* temp = NULL;
	LoadTile(temp, "./interface/MJf1-.svg");
	Est = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (Est == NULL)
	{
		std::cout << stderr << "could not create Est: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, Est, NULL);
	}
	SDL_FreeSurface(temp);

	temp = NULL;
	LoadTile(temp, "./interface/MJf2-.svg");
	Sud = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (Sud == NULL)
	{
		std::cout << stderr << "could not create Sud: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, Sud, NULL);
	}
	SDL_FreeSurface(temp);

	temp = NULL;
	LoadTile(temp, "./interface/MJf3-.svg");
	Ouest = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (Ouest == NULL)
	{
		std::cout << stderr << "could not create Ouest: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, Ouest, NULL);
	}
	SDL_FreeSurface(temp);

	temp = NULL;
	LoadTile(temp, "./interface/MJf4-.svg");
	Nord = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (Nord == NULL)
	{
		std::cout << stderr << "could not create Nord: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, Nord, NULL);
	}
	SDL_FreeSurface(temp);

	temp = NULL;
	LoadTile(temp, "./interface/MJd3rv1-.svg");
	turn = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (turn == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, turn, NULL);
	}

	temp = NULL;
	LoadTile(temp, "./interface/blank.svg");
	hint = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (hint == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, hint, NULL);
	}

	temp = NULL;
	LoadTile(temp, "./interface/blank.svg");
	restart = SDL_CreateRGBSurface(0, temp->w / 2, temp->h / 2, temp->format->BitsPerPixel, temp->format->Rmask, temp->format->Gmask, temp->format->Bmask, temp->format->Amask);
	if (restart == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_BlitScaled(temp, NULL, restart, NULL);
	}
}

void GraphicBoard::setClicked(const int x, const int y)
{
	Uint8* p = (Uint8*)mousescreen->pixels + y * mousescreen->pitch + x * mousescreen->format->BytesPerPixel;
	Uint32 index = *(Uint32*)p & 0xFF;
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
						int left = plateau.getDominoFromIndex(selected);
						int right = plateau.getDominoFromIndex(index);
						if (
							left == right ||
							(34 <= left && left < 38 && 34 <= right && right < 38) || // Saisons
							(38 <= left && left < 42 && 38 <= right && right < 42) // Fleurs.
							)
						{
							clicked[index] = true;
							plateau.RemovePairOfTiles(selected, index);
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
				else if(direction == 2)
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
	}
}

void GraphicBoard::RefreshMouseMap()
{
	// Copie du fond :
	SDL_FillRect(virtualmousescreen, NULL, SDL_MapRGB(virtualmousescreen->format, 0xFF, 0xFF, 0xFF));

	/**/
	// Placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (bordermask->w - 40) * 12) >> 1;
	auto tHeight = (Height - (bordermask->h - 40) >> 3) >> 1;

	for (auto& tile : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(tile);
		auto y = std::get<1>(tile);
		auto z = std::get<2>(tile);
		auto domino = std::get<3>(tile);
		auto index = std::get<4>(tile);
		if (direction == 3)
		{
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) + z * 40 + tHeight;
			SDL_SetColourOnOpaque(dominos[domino], virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, index, 0x00, 0x00));
		}
		else if(direction == 0)
		{
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			// Récupération du domino :
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_VerticalFlip(temp);

			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, index, 0x00, 0x00));

			SDL_FreeSurface(temp);
		}
		else if (direction == 1)
		{
			coordonnees.x = x * (bordermask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;
			// Up - Right
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);
			
			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, index, 0x00, 0x00));

			SDL_FreeSurface(temp);
		}
		else
		{
			// Down - Right
			coordonnees.x = x * (bordermask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) + z * 40 + tHeight;
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);
			SDL_VerticalFlip(temp);

			SDL_SetColourOnOpaque(temp, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, index, 0x00, 0x00));

			SDL_FreeSurface(temp);
		}
	}

	// Ouest :
	coordonnees.x = 0;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_SetColourOnOpaque(Ouest, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, WEST, 0x00, 0x00));
	// Sud :
	coordonnees.x = Nord->w - 20;
	coordonnees.y = (restart->h - 20) * 5;
	SDL_SetColourOnOpaque(Sud, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, SOUTH, 0x00, 0x00));
	// Turn :
	coordonnees.x = restart->w - 20;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_SetColourOnOpaque(turn, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, TURN, 0x00, 0x00));
	// Nord :
	coordonnees.x = restart->w - 20;
	coordonnees.y = (restart->h - 20) * 3;
	SDL_SetColourOnOpaque(Nord, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, NORTH, 0x00, 0x00));
	// Est :
	coordonnees.x = (restart->w << 1) - 40;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_SetColourOnOpaque(Est, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, EAST, 0x00, 0x00));
	// Hint :
	coordonnees.x = restart->w - 20;
	coordonnees.y = restart->h - 20;
	SDL_SetColourOnOpaque(hint, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, HINT, 0x00, 0x00));
	// Restart :
	coordonnees.x = restart->w - 20;
	coordonnees.y = 0;
	SDL_SetColourOnOpaque(restart, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, RESTART, 0x00, 0x00));


	SDL_BlitScaled(virtualmousescreen, NULL, mousescreen, NULL);
}

#ifdef _DEBUG
void GraphicBoard::RefreshExample()
{
	// Copie du fond :
	//SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	SDL_BlitScaled(background, NULL, virtualscreen, NULL);

	// Placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (bordermask->w - 40) * 12) >> 1;
	auto tHeight = (Height - (bordermask->h - 40) >> 3) >> 1;

	auto& tile = *plateau.getLogicalBoard().begin();
	{
		/**/
		auto x = std::get<0>(tile);
		auto y = std::get<1>(tile);
		auto z = std::get<2>(tile);
		auto domino = std::get<3>(tile);
		auto index = std::get<4>(tile);

		/**/
		/**/
		{
			x += 2;
			SDL_Rect coord;
			coord.x = 0;
			coord.y = 0;
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;

			coordonnees.y = (y - 2) * (bordermask->h - 40) - z * 40 + tHeight;
			SDL_UpperBlit(dominos[0], NULL, virtualscreen, &coordonnees);

			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;
			SDL_UpperBlit(facedown, NULL, virtualscreen, &coordonnees);

			coordonnees.y = (y + 2) * (bordermask->h - 40) - z * 40 + tHeight;
			auto tempBorderMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, tempBorderMask, NULL);
			SDL_UpperBlit(tempBorderMask, NULL, virtualscreen, &coordonnees);
			SDL_FreeSurface(tempBorderMask);
		}
		/**/
		/**/
		{

			x += 2;
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;

			SDL_Rect coord;
			coord.x = 0;
			coord.y = 0;

			auto tempMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			// Récupération du domino :
			SDL_UpperBlit(dominos[0], NULL, tempMask, NULL);
			// Découpage de tout sauf de la face :
			SDL_UpperBlitCut(facedown, tempMask);
			coordonnees.y = (y - 1) * (bordermask->h - 40) - z * 40 + tHeight;
			SDL_UpperBlit(tempMask, NULL, virtualscreen, &coordonnees);

			auto tempBorderMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, tempBorderMask, NULL);
			SDL_VerticalFlip(tempBorderMask);
			coordonnees.y = (y + 2) * (bordermask->h - 40) - z * 40 + tHeight;
			SDL_UpperBlit(tempBorderMask, NULL, virtualscreen, &coordonnees);

			SDL_FreeSurface(tempMask);
			SDL_FreeSurface(tempBorderMask);
		}
		/**/
		/**/
		{
			auto tempBorderMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, tempBorderMask, NULL);
			SDL_VerticalFlip(tempBorderMask);
			auto tempMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			x += 2;
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_Rect coord;
			coord.x = 0;
			coord.y = -38;

			// Récupération du domino :
			SDL_UpperBlit(dominos[0], NULL, tempMask, NULL);
			// Découpage de tout sauf de la face :
			SDL_UpperBlitCut(facedown, tempMask);
			// Dessin du domino sans face :
			SDL_UpperBlit(tempBorderMask, NULL, temp, NULL);
			// Dessin de la face sur de domino :
			SDL_UpperBlit(tempMask, NULL, temp, &coord);

			SDL_UpperBlit(temp, NULL, virtualscreen, &coordonnees);

			SDL_FreeSurface(temp);
			SDL_FreeSurface(tempMask);
			SDL_FreeSurface(tempBorderMask);
		}
		/**/
	}

	// Interface :
	coordonnees.x = 0;
	coordonnees.y = 0;
	SDL_UpperBlit(restart, NULL, virtualscreen, &coordonnees);
	coordonnees.x = 0;
	coordonnees.y = 100;
	SDL_UpperBlit(hint, NULL, virtualscreen, &coordonnees);
	coordonnees.x = 0;
	coordonnees.y = 200;
	SDL_UpperBlit(turn, NULL, virtualscreen, &coordonnees);

	SDL_BlitScaled(virtualscreen, NULL, tampon, NULL);

	SDL_RenderClear(renderer);
	auto texture = SDL_CreateTextureFromSurface(renderer, tampon);
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

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);
}
#endif

void GraphicBoard::Refresh(bool refreshMouseMap)
{
#ifdef _DEBUG
	//return RefreshExample();
#endif
	if (refreshMouseMap) RefreshMouseMap();
	// Copie du fond :
	//SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	SDL_BlitScaled(background, NULL, virtualscreen, NULL);

	// Placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (bordermask->w - 40) * 12) >> 1;
	auto tHeight = (Height - (bordermask->h - 40) >> 3) >> 1;

	for (auto& tile : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(tile);
		auto y = std::get<1>(tile);
		auto z = std::get<2>(tile);
		auto domino = std::get<3>(tile);
		auto index = std::get<4>(tile);
		if (direction == 3)
		{
			// Down - Left
			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) + z * 40 + tHeight;
			if (clicked[index])
				SDL_UpperBlitInverted(dominos[domino], virtualscreen, coordonnees);
			else
				SDL_UpperBlit(dominos[domino], NULL, virtualscreen, &coordonnees);
		}
		else if(direction == 0)
		{
			// Up - Left
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_VerticalFlip(temp);
			auto tempMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			coordonnees.x = x * (bordermask->w - 40) - z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;

			SDL_Rect coord;
			coord.x = 0;
			coord.y = -38;

			// Récupération du domino :
			SDL_UpperBlit(dominos[domino], NULL, tempMask, NULL);
			// Découpage de tout sauf de la face :
			SDL_UpperBlitCut(facedown, tempMask);
			// Dessin du domino sans face sur le domino vierge :
			SDL_UpperBlit(tempMask, NULL, temp, &coord);

			if (clicked[index])
				SDL_UpperBlitInverted(temp, virtualscreen, coordonnees);
			else
				SDL_UpperBlit(temp, NULL, virtualscreen, &coordonnees);

			SDL_FreeSurface(temp);
			SDL_FreeSurface(tempMask);
		}
		else if (direction == 1)
		{
			// Up - Right
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);
			auto tempMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			coordonnees.x = x * (bordermask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) - z * 40 + tHeight;

			SDL_Rect coord;
			coord.x = 33;
			coord.y = -38;

			// Récupération du domino :
			SDL_UpperBlit(dominos[domino], NULL, tempMask, NULL);
			// Découpage de tout sauf de la face :
			SDL_UpperBlitCut(facedown, tempMask);
			// Dessin du domino sans face sur le domino vierge :
			SDL_UpperBlit(tempMask, NULL, temp, &coord);

			if (clicked[index])
				SDL_UpperBlitInverted(temp, virtualscreen, coordonnees);
			else
				SDL_UpperBlit(temp, NULL, virtualscreen, &coordonnees);

			SDL_FreeSurface(temp);
			SDL_FreeSurface(tempMask);
		}
		else
		{
			// Down - Right
			auto temp = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
			SDL_UpperBlit(bordermask, NULL, temp, NULL);
			SDL_HorizontalFlip(temp);
			SDL_VerticalFlip(temp);
			auto tempMask = SDL_CreateRGBSurface(0, bordermask->w, bordermask->h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

			coordonnees.x = x * (bordermask->w - 40) + z * 40 + tWidth;
			coordonnees.y = y * (bordermask->h - 40) + z * 40 + tHeight;

			SDL_Rect coord;
			coord.x = 33;
			coord.y = 0;

			// Récupération du domino :
			SDL_UpperBlit(dominos[domino], NULL, tempMask, NULL);
			// Découpage de tout sauf de la face :
			SDL_UpperBlitCut(facedown, tempMask);
			// Dessin du domino sans face sur le domino vierge :
			SDL_UpperBlit(tempMask, NULL, temp, &coord);

			if (clicked[index])
				SDL_UpperBlitInverted(temp, virtualscreen, coordonnees);
			else
				SDL_UpperBlit(temp, NULL, virtualscreen, &coordonnees);

			SDL_FreeSurface(temp);
			SDL_FreeSurface(tempMask);
		}
	}

	/*
	SDL_Rect coordonnees;
	int i = 0;
	auto tHeight = (Height - (bordermask->h - 40) * 6) / 2;
	auto tWidth = (Width - (bordermask->w - 40) * 7) / 2;
	for (int z = 0; z < 5; ++z)
	{
		int i = 0;
		for (int y = 5; y >= 0; --y)
		{
			for (int x = 0; x < 7; ++x)
			{
				coordonnees.x = x * (bordermask->w - 40 ) - z * 40 + tWidth;
				coordonnees.y = y * (bordermask->h - 40) + z * 40 + tHeight;
				SDL_UpperBlit(dominos[i++], NULL, virtualscreen, &coordonnees);
			}
		}
	}
	/**/

	if (plateau.IsBlocked())
	{
		if (plateau.IsEmpty())
		{
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
		}
		else
		{
			SDL_SetGreyScale(virtualscreen);
		}
	}

	// Interface :
	// Ouest :
	coordonnees.x = 0;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_UpperBlit(Ouest, NULL, virtualscreen, &coordonnees);
	// Sud :
	coordonnees.x = Nord->w - 20;
	coordonnees.y = (restart->h - 20) * 5;
	SDL_UpperBlit(Sud, NULL, virtualscreen, &coordonnees);
	// Turn :
	coordonnees.x = restart->w - 20;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_UpperBlit(turn, NULL, virtualscreen, &coordonnees);
	// Nord :
	coordonnees.x = restart->w - 20;
	coordonnees.y = (restart->h - 20) * 3;
	SDL_UpperBlit(Nord, NULL, virtualscreen, &coordonnees);
	// Est :
	coordonnees.x = (restart->w << 1) - 40;
	coordonnees.y = (restart->h - 20) * 4;
	SDL_UpperBlit(Est, NULL, virtualscreen, &coordonnees);
	// Hint :
	coordonnees.x = restart->w - 20;
	coordonnees.y = restart->h - 20;
	SDL_UpperBlit(hint, NULL, virtualscreen, &coordonnees);
	// Restart :
	coordonnees.x = restart->w - 20;
	coordonnees.y = 0;
	SDL_UpperBlit(restart, NULL, virtualscreen, &coordonnees);
	/**/

#ifdef _DEBUG
	SDL_BlitScaled(virtualscreen, NULL, tampon, NULL);
	//SDL_BlitScaled(virtualmousescreen, NULL, tampon, NULL);
#else
	SDL_BlitScaled(virtualscreen, NULL, tampon, NULL);
#endif

	SDL_RenderClear(renderer);
	auto texture = SDL_CreateTextureFromSurface(renderer, tampon);
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

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);
}

void GraphicBoard::WhatsLeft()
{
	// copie du fond :
	//SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	SDL_BlitScaled(background, NULL, virtualscreen, NULL);
	/**/
	auto tWitdh = (Width - (bordermask->h + 40 * 3)) >> 3;
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
			int domino = y + x * 8;
			int t = board[y][x];
			for (int z = 0; z < board[y][x]; ++z)
			{
				coordonnees.x = x * (bordermask->h) + z * 40 + x * tWitdh;
				coordonnees.y = y * (bordermask->h - 40);
				SDL_UpperBlit(dominos[domino], NULL, virtualscreen, &coordonnees);
			}
		}
	/**/

	SDL_BlitScaled(virtualscreen, NULL, tampon, NULL);

	SDL_RenderClear(renderer);
	auto texture = SDL_CreateTextureFromSurface(renderer, tampon);
	if (texture == NULL)
	{
		std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0)
	{
		SDL_DestroyTexture(texture);
		std::cout << stderr << "could not copy to renderer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);
}

void GraphicBoard::Loop()
{
	if (bInitDone)
	{
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
					done = true;
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
	else
	{
		ThrowException(1);
	}
}
