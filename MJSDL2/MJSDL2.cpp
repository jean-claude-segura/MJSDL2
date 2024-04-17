#include "GraphicBoard.h"
#include <windows.h>
#ifndef _DEBUG
#include <windows.h>
#endif
int main()
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	try
	{
		// https://stackoverflow.com/questions/31162367/significance-of-ios-basesync-with-stdiofalse-cin-tienull
		std::ios_base::sync_with_stdio(false);
		std::cin.tie(NULL); 
#ifndef _DEBUG
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
		GraphicBoard plateau;
		plateau.Loop();
	}
	catch (...)
	{
	}

#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
	return 0;
}
