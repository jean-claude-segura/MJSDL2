#include "GraphicBoard.h"
#ifdef _RELEASE
#include <windows.h>
#endif
int main()
{
	try
	{
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
