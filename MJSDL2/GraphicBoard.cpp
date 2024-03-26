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
	
	virtualscreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0, 0, 0, 0);
	if(virtualscreen == NULL)
	{
		std::cout << stderr << "could not create virtual screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	virtualmousescreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0, 0, 0, 0);
	if (virtualmousescreen == NULL)
	{
		std::cout << stderr << "could not create virtual mouse screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);
	if (tampon == NULL)
	{
		std::cout << stderr << "could not create buffer: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	mousescreen = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);
	if (mousescreen == NULL)
	{
		std::cout << stderr << "could not create mouse screen: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}

	LoadResources();
	
	Refresh();

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);
}

GraphicBoard::~GraphicBoard()
{
	FreeResources();
}

void GraphicBoard::ThrowException(int i)
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
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void GraphicBoard::LoadResources()
{
	// Bambous : 9*4
	for (int i = 0; i < 9; ++i)
	{
		std::string path = "./tiles/MJs";
		path += '1' + i;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Cercles : 9*4
	for (int i = 9; i < 18; ++i)
	{
		std::string path = "./tiles/MJt";
		path += '1' + i - 9;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Caractères : 9*4
	for (int i = 18; i < 27; ++i)
	{
		std::string path = "./tiles/MJw";
		path += '1' + i - 18;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Honneurs : 4*4
	for (int i = 27; i < 31; ++i)
	{
		std::string path = "./tiles/MJf";
		path += '1' + i - 27;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Dragons : 3*4
	for (int i = 31; i < 34; ++i)
	{
		std::string path = "./tiles/MJd";
		path += '1' + i - 31;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Saisons : 1 * 4
	for (int i = 34; i < 38; ++i)
	{
		std::string path = "./tiles/MJh";
		path += '1' + i - 34;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	// Fleurs : 1 * 4
	for (int i = 38; i < 42; ++i)
	{
		std::string path = "./tiles/MJh";
		path += '1' + i - 34;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
		if (dominos[i] == NULL)
		{
			std::cout << stderr << "could not create tile " << i << " : " << SDL_GetError() << std::endl;
			ThrowException(1);
		}
	}
	background = IMG_Load("./background/vecteezy_wood-texture-background-wood-pattern-texture_2680573.jpg");
	if (background == NULL) {
		std::cout << stderr << "could not create surface: " << SDL_GetError() << std::endl;
		ThrowException(1);
	}
}

void GraphicBoard::setClicked(int x, int y)
{
	Uint32*  lpMouse = (Uint32*)mousescreen;
	Uint32 colour;
	int bpp = mousescreen->format->BytesPerPixel;
	Uint8* p = (Uint8*)mousescreen->pixels + y * mousescreen->pitch + x * bpp;
	colour = *(Uint32*)p;

	std::cout << "0x" << std::hex << colour << std::endl;
}

void GraphicBoard::Refresh()
{
	// copie du fond :
	SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	SDL_FillRect(virtualmousescreen, NULL, SDL_MapRGBA(virtualmousescreen->format, 0xFF, 0xFF, 0xFF, 0x00));

	/**/
	// placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (dominos[0]->w - 40) * 12) >> 1;
	auto tHeight = (Height - (dominos[0]->h - 40) >> 3) >> 1;

	coordonnees.x = -1 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	SDL_UpperBlit(dominos[plateau.getSpeciaux()[0]], NULL, virtualscreen, &coordonnees);
	SDL_SetColourOnOpaque(dominos[plateau.getSpeciaux()[0]], virtualmousescreen, coordonnees, SDL_MapRGBA(virtualmousescreen->format, plateau.getSpeciaux()[0], 0x00, 0x00, 0x00));

	for (int z = 0; z < 5; ++z)
	{
		for (int y = 7; y >= 0; --y)
		{
			for (int x = 0; x < 12; ++x)
			{
				int index = 0;
				for (; index < 140; ++index)
				{
					if (
						std::get<0>(plateau.getBoard()[index]) == x &&
						std::get<1>(plateau.getBoard()[index]) == y &&
						std::get<2>(plateau.getBoard()[index]) == z
						) break;
				}
				if (index != 140)
				{
					coordonnees.x = x * (dominos[0]->w - 40) - z * 40 + tWidth;
					coordonnees.y = y * (dominos[0]->h - 40) + z * 40 + tHeight;
					SDL_UpperBlit(dominos[std::get<3>(plateau.getBoard()[index])], NULL, virtualscreen, &coordonnees);
					SDL_SetColourOnOpaque(dominos[std::get<3>(plateau.getBoard()[index])], virtualmousescreen, coordonnees, SDL_MapRGBA(virtualmousescreen->format, std::get<3>(plateau.getBoard()[index]), 0x00, 0x00,0x00));
				}
			}
		}
	}

	coordonnees.x = 12 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	//SDL_UpperBlit(dominos[plateau.getSpeciaux()[1]], NULL, virtualscreen, &coordonnees);
	SDL_UpperBlitXored(dominos[plateau.getSpeciaux()[1]], virtualscreen, coordonnees);
	SDL_SetColourOnOpaque(dominos[plateau.getSpeciaux()[1]], virtualmousescreen, coordonnees, SDL_MapRGBA(virtualmousescreen->format, plateau.getSpeciaux()[1], 0x00, 0x00, 0x00));

	coordonnees.x = 13 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	//SDL_UpperBlit(dominos[plateau.getSpeciaux()[2]], NULL, virtualscreen, &coordonnees);
	SDL_UpperBlitNegate(dominos[plateau.getSpeciaux()[2]], virtualscreen, coordonnees);
	SDL_SetColourOnOpaque(dominos[plateau.getSpeciaux()[2]], virtualmousescreen, coordonnees, SDL_MapRGBA(virtualmousescreen->format, plateau.getSpeciaux()[2], 0x00, 0x00, 0x00));


	coordonnees.x = 5.5 * (dominos[0]->w - 40) - 4 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 4 * 40 + tHeight;
	//SDL_UpperBlit(dominos[plateau.getSpeciaux()[3]], NULL, virtualscreen, &coordonnees);
	SDL_UpperBlitInverted(dominos[plateau.getSpeciaux()[3]], virtualscreen, coordonnees);
	SDL_SetColourOnOpaque(dominos[plateau.getSpeciaux()[3]], virtualmousescreen, coordonnees, SDL_MapRGBA(virtualmousescreen->format, plateau.getSpeciaux()[3], 0x00, 0x00, 0x00));
	//SDL_UpperBlitXored(dominos[plateau.getSpeciaux()[3]], virtualscreen, coordonnees);

	/**/
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

	SDL_BlitScaled(virtualmousescreen, NULL, mousescreen, &ScreenRect);

	SDL_BlitScaled(virtualscreen, NULL, tampon, &ScreenRect);

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
	SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	/**/
	auto tWitdh = (Width - (dominos[0]->h + 40 * 3)) >> 3;
	SDL_Rect coordonnees;
	int board[8][6];
	for (int x = 0; x < 6; ++x)
		for (int y = 0; y < 8; ++y)
			board[y][x] = 0;
	for (int index = 0; index < 140; ++index)
	{
		int domino = std::get<3>(plateau.getBoard()[index]);
		int x = (domino >> 3);
		int y = (domino & 0b111);
		++board[y][x];
	}
	for (int index = 0; index < 4; ++index)
	{
		int domino = plateau.getSpeciaux()[index];
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

	SDL_BlitScaled(virtualscreen, NULL, tampon, &ScreenRect);

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
					setClicked(event.motion.x, event.motion.y);
					//plateau.InitBoard();
					//Refresh();
					switch (event.type)
					{
					case SDL_MOUSEMOTION:
						break;
					default:
						break;
					}
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