#include "GraphicBoard.h"

GraphicBoard::GraphicBoard()
{
	SDL_SetMainReady();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << stderr << "could not initialize sdl2 : " << SDL_GetError() << std::endl;
		throw;
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
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN // SDL_WINDOW_SHOWN
		//SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL // SDL_WINDOW_SHOWN
		SDL_WINDOW_SHOWN
	);
	if (window == NULL) {
		std::cout << stderr << "could not create window: " << SDL_GetError() << std::endl;
		throw;
	}


	//background = SDL_CreateRGBSurface(0, Width, Height, 32, 0, 0, 0, 0);
	//dominos = std::make_unique<SDL_Surface * []>(42);
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		std::cout << stderr << "could not create renderer: " << SDL_GetError() << std::endl;
		throw;
	}

	LoadResources();
	plateau.FillBoard();
	Refresh();

	/*screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));*/
	//SDL_UpdateWindowSurface(window);
}

GraphicBoard::~GraphicBoard()
{
	for (int i = 0; i < 42; ++i)
	{
		if(dominos[i] != NULL)
			SDL_FreeSurface(dominos[i]);
	}

	SDL_FreeSurface(background);
	if (window != NULL)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void GraphicBoard::LoadResources()
{
	// Cercles : 9*4
	for (int i = 0; i < 9; ++i)
	{
		std::string path = "./tiles/MJt";
		path += '1' + i;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Bambous : 9*4
	for (int i = 9; i < 18; ++i)
	{
		std::string path = "./tiles/MJs";
		path += '1' + i - 9;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Caractères : 9*4
	for (int i = 18; i < 27; ++i)
	{
		std::string path = "./tiles/MJw";
		path += '1' + i - 18;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Saisons : 1 * 4
	for (int i = 27; i < 31; ++i)
	{
		std::string path = "./tiles/MJh";
		path += '1' + i - 27;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Fleurs : 1 * 4
	for (int i = 31; i < 35; ++i)
	{
		std::string path = "./tiles/MJh";
		path += '1' + i - 27;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Honeurs : 4*4
	for (int i = 35; i < 39; ++i)
	{
		std::string path = "./tiles/MJf";
		path += '1' + i - 35;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	// Dragons : 3*4
	for (int i = 39; i < 42; ++i)
	{
		std::string path = "./tiles/MJd";
		path += '1' + i - 39;
		path += "-.svg";
		dominos[i] = IMG_Load(path.c_str());
	}
	background = IMG_Load("./background/vecteezy_wood-texture-background-wood-pattern-texture_2680573.jpg");
	if (background == NULL) {
		std::cout << stderr << "could not create surface: " << SDL_GetError() << std::endl;
		throw 1;
	}
}

void GraphicBoard::setClicked(int x, int y)
{
	auto tWidth = (Width - (dominos[0]->w - 40) * 12) / 2;
	auto tHeight = (Height - (dominos[0]->h - 40) * 8) / 2;
	//coordonnees.x = x * (dominos[0]->w - 40) - z * 40 + tWidth;
	//coordonnees.y = y * (dominos[0]->h - 40) + z * 40 + tHeight;

}

void GraphicBoard::Refresh()
{
	SDL_Surface* virtualscreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0, 0, 0, 0);
	// copie du fond :
	SDL_UpperBlit(background, NULL, virtualscreen, NULL);

	/**/
	// placement des dominos :
	SDL_Rect coordonnees;

	auto tWidth = (Width - (dominos[0]->w - 40) * 12) / 2;
	auto tHeight = (Height - (dominos[0]->h - 40) * 8) / 2;

	coordonnees.x = -1 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	SDL_UpperBlit(dominos[plateau.getSpeciaux()[0]], NULL, virtualscreen, &coordonnees);

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
				}
			}
		}
	}

	coordonnees.x = 12 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	SDL_UpperBlit(dominos[plateau.getSpeciaux()[1]], NULL, virtualscreen, &coordonnees);

	coordonnees.x = 13 * (dominos[0]->w - 40) - 0 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 0 * 40 + tHeight;
	SDL_UpperBlit(dominos[plateau.getSpeciaux()[2]], NULL, virtualscreen, &coordonnees);


	coordonnees.x = 5.5 * (dominos[0]->w - 40) - 4 * 40 + tWidth;
	coordonnees.y = 3.5 * (dominos[0]->h - 40) + 4 * 40 + tHeight;
	//SDL_UpperBlit(dominos[plateau.getSpeciaux()[3]], NULL, virtualscreen, &coordonnees);
	SDL_UpperBlitInverted(dominos[plateau.getSpeciaux()[3]], virtualscreen, coordonnees);		

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
	SDL_Surface* tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);

	SDL_BlitScaled(virtualscreen, NULL, tampon, &ScreenRect);

	SDL_RenderClear(renderer);
	auto texture = SDL_CreateTextureFromSurface(renderer, tampon);
	if (renderer == NULL)
	{
		SDL_FreeSurface(tampon);
		SDL_FreeSurface(virtualscreen);

		std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
		throw;
	}
	if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0)
	{
		SDL_DestroyTexture(texture);

		std::cout << stderr << "could not copy renderer: " << SDL_GetError() << std::endl;
		throw;
	}

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);

	SDL_FreeSurface(tampon);

	SDL_FreeSurface(virtualscreen);
}

void GraphicBoard::WhatsLeft()
{
	SDL_Surface* virtualscreen = SDL_CreateRGBSurface(0, Width, Height, 32, 0, 0, 0, 0);
	// copie du fond :
	SDL_UpperBlit(background, NULL, virtualscreen, NULL);
	/**/
	int marques[42];
	for (int m = 0; m < 42; ++m) marques[m] = 0;
	
	auto tWitdh = (Width - (dominos[0]->h + 40 * 3)) >> 3;
	SDL_Rect coordonnees;
	for (int index = 0; index < 140; ++index)
	{
		int domino = std::get<3>(plateau.getBoard()[index]);
		coordonnees.x = (domino >> 3) * (dominos[0]->h) + marques[domino] * 40 + (domino >> 3) * tWitdh;
		++marques[domino];
		coordonnees.y = (domino % 8) * (dominos[0]->h - 40);
		SDL_UpperBlit(dominos[domino], NULL, virtualscreen, &coordonnees);
	}
	for (int index = 0; index < 4; ++index)
	{
		int domino = plateau.getSpeciaux()[index];
		coordonnees.x = (domino >> 3) * (dominos[0]->h) + marques[domino] * 40 + (domino >> 3) * tWitdh;
		++marques[domino];
		coordonnees.y = (domino % 8) * (dominos[0]->h - 40);
		SDL_UpperBlit(dominos[domino], NULL, virtualscreen, &coordonnees);
	}
	/**/

	SDL_Surface* tampon = SDL_CreateRGBSurface(0, ScreenRect.w, ScreenRect.h, 32, 0, 0, 0, 0);

	SDL_BlitScaled(virtualscreen, NULL, tampon, &ScreenRect);

	SDL_RenderClear(renderer);
	auto texture = SDL_CreateTextureFromSurface(renderer, tampon);
	if (renderer == NULL)
	{
		SDL_FreeSurface(tampon);
		SDL_FreeSurface(virtualscreen);

		std::cout << stderr << "could not create texture: " << SDL_GetError() << std::endl;
		throw;
	}

	if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0)
	{
		SDL_DestroyTexture(texture);
		std::cout << stderr << "could not copy to renderer: " << SDL_GetError() << std::endl;
		throw;
	}

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);

	SDL_ShowCursor(true ? SDL_ENABLE : SDL_DISABLE);

	SDL_FreeSurface(tampon);
	SDL_FreeSurface(virtualscreen);
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
					Refresh();
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
				//plateau.FillBoard();
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