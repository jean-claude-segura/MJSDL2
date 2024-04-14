#include "GraphicBoard.h"
#ifdef _RELEASE
#include <windows.h>
#endif
int main()
{
	try
	{
		// https://stackoverflow.com/questions/31162367/significance-of-ios-basesync-with-stdiofalse-cin-tienull
		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL); 
#ifdef _RELEASE
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
		GraphicBoard plateau;
		plateau.Loop();
	}
	catch (...)
	{
	}

#ifdef _RELEASE
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
	return 0;
}
