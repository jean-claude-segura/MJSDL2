#include "GraphicBoard.h"

GraphicBoard::GraphicBoard()
{
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
	if(virtualscreen == NULL)
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

	//tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);
	tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (tampon == NULL)
	{
		std::cout << stderr << "could not create buffer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	mousescreen = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	//mousescreen = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);
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
	RefreshMouseMap();
	Refresh();
}

GraphicBoard::~GraphicBoard()
{
	FreeResources();
}

void GraphicBoard::ThrowException(const int i)
{
	FreeResources();
	throw i;
}

void GraphicBoard::FreeResources()
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
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_Quit();
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
		auto temp = IMG_Load(it->c_str());
		dominos[i] = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
		SDL_FreeSurface(temp);
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
void GraphicBoard::LoadBackground(const std::string& path)
{
	auto temp = IMG_Load(path.c_str());
	background = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
	if (background == NULL) {
		std::cout << stderr << "could not create surface: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	SDL_FreeSurface(temp);
}

void GraphicBoard::LoadResources()
{
	// Bambous : 9*4
	LoadRamdomTileSet(0, 9, "./tiles/Bambous/");

	// Cercles : 9*4
	LoadRamdomTileSet(9, 18, "./tiles/Cercles/");
	
	// Caractères : 9*4
	LoadRamdomTileSet(18, 27, "./tiles/Caracteres/");
	
	// Honneurs : 4*4
	LoadRamdomTileSet(27, 31, "./tiles/Honneurs/");
	
	// Dragons : 3*4
	LoadRamdomTileSet(31, 34, "./tiles/Dragons/");
	
	// Saisons : 1 * 4
	LoadRamdomTileSet(34, 38, "./tiles/Saisons/");
	
	// Fleurs : 1 * 4
	LoadRamdomTileSet(38, 42, "./tiles/Fleurs/");

	LoadBackground("./background/10013168.jpg");
	//LoadBackground("./background/vecteezy_wood-texture-background-wood-pattern-texture_2680573.jpg");
}

void GraphicBoard::LoadUI()
{
	hint = SDL_CreateRGBSurface(0, 100, 100, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (hint == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_FillRect(hint, NULL, SDL_MapRGBA(hint->format, 0x00, 0xFF, 0x00, 0xFF));
	}
	restart = SDL_CreateRGBSurface(0, 100, 100, 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
	if (restart == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
	else
	{
		SDL_FillRect(restart, NULL, SDL_MapRGBA(restart->format, 0x00, 0x00, 0xFF, 0xFF));
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
						Refresh();
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
							RefreshMouseMap();
							Refresh();
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
					Refresh();
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
				SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
				plateau.InitBoard();
				itNextMove = plateau.GetMovesLeft().begin();
				RefreshMouseMap();
				Refresh();
				SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
				break;
			case HINT:
				SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
				if (itNextMove == plateau.GetMovesLeft().end())
					itNextMove = plateau.GetMovesLeft().begin();
				if (itNextMove != plateau.GetMovesLeft().end())
				{
					clicked[selected] = false;
					selected = -1;
					clicked[itNextMove->first] = true;
					clicked[itNextMove->second] = true;
					Refresh();
					SDL_Delay(300);
					clicked[itNextMove->first] = false;
					clicked[itNextMove->second] = false;
					Refresh();
					++itNextMove;
				}
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

	auto tWidth = (Width - (dominos[0]->w - 40) * 12) >> 1;
	auto tHeight = (Height - (dominos[0]->h - 40) >> 3) >> 1;

	for (auto& temp : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(temp);
		auto y = std::get<1>(temp);
		auto z = std::get<2>(temp);
		auto domino = std::get<3>(temp);
		auto index = std::get<4>(temp);
		coordonnees.x = x * (dominos[0]->w - 40) - z * 40 + tWidth;
		coordonnees.y = y * (dominos[0]->h - 40) + z * 40 + tHeight;
		SDL_SetColourOnOpaque(dominos[domino], virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, index, 0x00, 0x00));
	}

	coordonnees.x = 0;
	coordonnees.y = 0;
	SDL_SetColourOnOpaque(restart, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, RESTART, 0x00, 0x00));
	coordonnees.x = 0;
	coordonnees.y = 100;
	SDL_SetColourOnOpaque(hint, virtualmousescreen, coordonnees, SDL_MapRGB(virtualmousescreen->format, HINT, 0x00, 0x00));

	SDL_BlitScaled(virtualmousescreen, NULL, mousescreen, NULL);
}

void GraphicBoard::Refresh()
{
	// Copie du fond :
	//SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	SDL_BlitScaled(background, NULL, virtualscreen, NULL);

	// Placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (dominos[0]->w - 40) * 12) >> 1;
	auto tHeight = (Height - (dominos[0]->h - 40) >> 3) >> 1;

	coordonnees.x = -1 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;

	for (auto& temp : plateau.getLogicalBoard())
	{
		auto x = std::get<0>(temp);
		auto y = std::get<1>(temp);
		auto z = std::get<2>(temp);
		auto domino = std::get<3>(temp);
		auto index = std::get<4>(temp);
		coordonnees.x = x * (dominos[0]->w - 40) - z * 40 + tWidth;
		coordonnees.y = y * (dominos[0]->h - 40) + z * 40 + tHeight;
		if (clicked[index])
			SDL_UpperBlitInverted(dominos[domino], virtualscreen, coordonnees);
		else
			SDL_UpperBlit(dominos[domino], NULL, virtualscreen, &coordonnees);
	}

	/*
	SDL_Rect coordonnees;
	int i = 0;
	auto tHeight = (Height - (dominos[0]->h - 40) * 6) / 2;
	auto tWidth = (Width - (dominos[0]->w - 40) * 7) / 2;
	for (int z = 0; z < 5; ++z)
	{
		int i = 0;
		for (int y = 5; y >= 0; --y)
		{
			for (int x = 0; x < 7; ++x)
			{
				coordonnees.x = x * (dominos[0]->w - 40 ) - z * 40 + tWidth;
				coordonnees.y = y * (dominos[0]->h - 40) + z * 40 + tHeight;
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
	coordonnees.x = 0;
	coordonnees.y = 0;
	SDL_UpperBlit(restart, NULL, virtualscreen, &coordonnees);
	coordonnees.x = 0;
	coordonnees.y = 100;
	SDL_UpperBlit(hint, NULL, virtualscreen, &coordonnees);

	SDL_BlitScaled(virtualscreen, NULL, tampon, NULL);
	//SDL_BlitScaled(virtualmousescreen, NULL, tampon, NULL);

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
	auto tWitdh = (Width - (dominos[0]->h + 40 * 3)) >> 3;
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
				coordonnees.x = x * (dominos[0]->h) + z * 40 + x * tWitdh;
				coordonnees.y = y * (dominos[0]->h - 40);
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
				Refresh();
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