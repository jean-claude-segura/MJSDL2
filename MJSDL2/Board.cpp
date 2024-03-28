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

	for (int z = 0; z < 5; ++z)
		for (int y = 0; y < 8; ++y)
			for (int x = 0; x < 12; ++x)
				OccupationBoard[x][y][z] = -1;

	InitBoard();
}

void Board::InitBoard()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, 41);

	int tempDominos[42] = {
	4,4,4,4,4,4,4,4,4, // Bambous
	4,4,4,4,4,4,4,4,4, // Cercles
	4,4,4,4,4,4,4,4,4, // Caractères
	4,4,4,4, // Honneurs
	4,4,4, // Dragons
	1,1,1,1, // Saisons
	1,1,1,1 // Fleurs
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
		OccupationBoard[std::get<0>(LogicalBoard[index])][std::get<1>(LogicalBoard[index])][std::get<2>(LogicalBoard[index])] = index;
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
	for(int index = 0; index < 140; ++index) removable[0x0] = false;
	removable[0x0] = true;
	removable[0xb] = true;
	removable[0xc] = true;
	removable[0x13] = true;
	removable[0x14] = true;
	removable[0x1d] = true;
	removable[0x36] = true;
	removable[0x3f] = true;
	removable[0x40] = true;
	removable[0x47] = true;
	removable[0x48] = true;
	removable[0x53] = true;
	removable[0x54] = true;
	removable[0x59] = true;
	removable[0x5a] = true;
	removable[0x5f] = true;
	removable[0x60] = true;
	removable[0x65] = true;
	removable[0x66] = true;
	removable[0x6b] = true;
	removable[0x6c] = true;
	removable[0x71] = true;
	removable[0x72] = true;
	removable[0x77] = true;
	removable[0x78] = true;
	removable[0x7b] = true;
	removable[0x7c] = true;
	removable[0x7f] = true;
	removable[0x80] = true;
	removable[0x83] = true;
	removable[0x84] = true;
	removable[0x87] = true;
	removable[0x8c] = true;
	removable[0x8e] = true;
	removable[0x8f] = true;

	Blocked = false;

	Left.clear();
	for (int i = 0; i < 144; ++i) Left.emplace_back(i);
}

const std::array<std::tuple<int, int, int, int>, 140>& Board::getBoard()
{
	return LogicalBoard;
}

const std::array <int, 4>& Board::getSpeciaux()
{
	return Speciaux;
}

const std::array<bool, 144>& Board::getRemovable()
{
	return removable;
}

void Board::Remove(int index)
{
	if (index < 140)
	{
		std::get<3>(LogicalBoard[index]) = -1;
	}
	else
	{
		Speciaux[index - 140] = -1;
	}

	auto it = std::find(Left.begin(), Left.end(), index);
	if (it != Left.end())
		Left.erase(it);

	removable[index] = false;

	if (index == 0x8F)
	{
		removable[0x88] = true;
		removable[0x89] = true;
		removable[0x8A] = true;
		removable[0x8B] = true;
	}
	else if (index == 0x8C)
	{
		removable[0x1E] = true;
		removable[0x2A] = true;
	}
	else if (index == 0x8E)
	{
		removable[0x8D] = true;
	}
	else if (index == 0x8D)
	{
		removable[0x29] = true;
		removable[0x35] = true;
	}
	else
	{
		int x = std::get<0>(LogicalBoard[index]);
		int y = std::get<1>(LogicalBoard[index]);
		int z = std::get<2>(LogicalBoard[index]);
		OccupationBoard[x][y][z] = -1;
		if (x < 11 && OccupationBoard[x + 1][y][z] >= 0 && ( z > 3 || OccupationBoard[x + 1][y][z + 1] < 0 ))
			removable[OccupationBoard[x + 1][y][z]] = true;
		if (x > 0 && OccupationBoard[x - 1][y][z] >= 0 && (z > 3 || OccupationBoard[x - 1][y][z + 1] < 0))
			removable[OccupationBoard[x - 1][y][z]] = true;
		if (z > 0)
		{
			if (x < 11 && OccupationBoard[x + 1][y][z-1] < 0)
				removable[OccupationBoard[x][y][z-1]] = true;
			if (x > 0 && OccupationBoard[x - 1][y][z-1] < 0)
				removable[OccupationBoard[x][y][z-1]] = true;
		}
	}
	int max = 0;
	std::map<int, int> TotalMovesLeftByTile;
	for (int i = 0; i < 140 && max < 2; ++i)
	{
		if (removable[i])
		{
			auto tile = std::get<3>(LogicalBoard[i]);
			if (TotalMovesLeftByTile.contains(tile))
				TotalMovesLeftByTile[tile] += 1;
			else
				TotalMovesLeftByTile[tile] = 1;
			max = std::max(TotalMovesLeftByTile[tile], max);
		}
	}
	for (int i = 0; i < 4 && max < 2; ++i)
	{
		if (removable[i + 140])
		{
			auto tile = Speciaux[i];
			if (TotalMovesLeftByTile.contains(tile))
				TotalMovesLeftByTile[tile] += 1;
			else
				TotalMovesLeftByTile[tile] = 1;
			max = std::max(TotalMovesLeftByTile[tile], max);
		}
	}
	Blocked = max != 2;
}

void Board::Remove(int first, int second)
{
	Remove(first);
	Remove(second);
}
