#include "GraphicBoard.h"

int main()
{
	// https://archive.org/details/MahJonggVGA_1020#

	try
	{
		GraphicBoard plateau;
		plateau.Init();
		plateau.Loop();
	}
	catch (...)
	{

	}

	return 0;
}
