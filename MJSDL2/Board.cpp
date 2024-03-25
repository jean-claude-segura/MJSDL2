#include "Board.h"

Board::Board()
{
	for (int z = 0; z < 5; ++z)
		for (int y = 0; y < 8; ++y)
			for (int x = 0; x < 12; ++x)
				BasePattern[x][y][z] = -1;
	for (int x = 0; x < 12; ++x)
	{
		BasePattern[x][0][0] = 1;
		BasePattern[x][7][0] = 1;
		BasePattern[x][3][0] = 1;
		BasePattern[x][4][0] = 1;
	}

	for (int x = 2; x < 10; ++x)
	{
		BasePattern[x][1][0] = 1;
		BasePattern[x][6][0] = 1;
	}
	for (int x = 1; x < 11; ++x)
	{
		BasePattern[x][2][0] = 1;
		BasePattern[x][5][0] = 1;
	}
	for (int x = 3; x < 9; ++x)
		for (int y = 1; y < 7; ++y)
			BasePattern[x][y][1] = 1;

	for (int x = 4; x < 8; ++x)
		for (int y = 2; y < 6; ++y)
			BasePattern[x][y][2] = 1;

	for (int x = 5; x < 7; ++x)
		for (int y = 3; y < 5; ++y)
			BasePattern[x][y][3] = 1;

	int index = 0;
	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z] == 1) LogicalBoard[index++] = std::make_tuple(x, y, z, 0);
			}
		}
	}

	/*
	int cptr = 0;

	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z] == 1) ++cptr;
			}
		}
	}
	/**/

	InitBoard();
}

void Board::InitBoard()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, 41);

	int tempDominos[42] = {
	4,4,4,4,4,4,4,4,4, // Cercles
	4,4,4,4,4,4,4,4,4, // Bambous
	4,4,4,4,4,4,4,4,4, // Caractères
	1,1,1,1, // Saisons
	1,1,1,1, // Fleurs
	4,4,4,4, // Honeurs
	4,4,4 // Dragons
	};
	for (int index = 0; index < 140; ++index)
	{
		int domino = 0;
		do
		{
			domino = uniform_dist(e1);
		} while (tempDominos[domino] == 0);
		int debugdom = tempDominos[domino];
		--tempDominos[domino];
		std::get<3>(LogicalBoard[index]) = domino;
	}

#ifdef _DEBUG
	// Confirmation base :
	int tempConfirm[42];
	for (int i = 0; i < 42; ++i)
		tempConfirm[i] = 0;
	for (int index = 0; index < 140; ++index)
	{
		int domino = std::get<3>(LogicalBoard[index]);
		++tempConfirm[domino];
	}
#endif

	for (int index = 0; index < 4; ++index)
	{
		int domino = 0;
		do
		{
			domino = uniform_dist(e1);
		} while (tempDominos[domino] == 0);
		int debugdom = tempDominos[domino];
		--tempDominos[domino];
		Speciaux[index] = domino;
	}

#ifdef _DEBUG
	// Confirmation spéciaux :
	for (int index = 0; index < 4; ++index)
	{
		int domino = Speciaux[index];
		++tempConfirm[domino];
	}
#endif
}

const std::array<std::tuple<int, int, int, int>, 140>& Board::getBoard()
{
	return LogicalBoard;
}

const std::array <int, 4>& Board::getSpeciaux()
{
	return Speciaux;
}
