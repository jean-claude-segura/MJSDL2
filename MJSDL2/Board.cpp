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
				if (BasePattern[x][y][z] == 1) InitIndexToCoord[index++] = std::make_tuple(x, y, z);
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

bool Board::CompLogicalBoard(std::tuple<int, int, int, int, int> left, std::tuple<int, int, int, int, int> right)
{
	return ! (std::get<2>(left) > std::get<2>(right) ||
		std::get<2>(left) == std::get<2>(right) &&
		(std::get<1>(left) < std::get<1>(right) || std::get<1>(left) == std::get<1>(right) && std::get<0>(left) > std::get<0>(right)));
	return
		std::get<2>(left) > std::get<2>(right) ||
		std::get<1>(left) < std::get<1>(right) ||
		std::get<0>(left) > std::get<0>(right);
}

void Board::InitBoard()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, 41);

	LogicalBoard.clear();
	TilesMap.clear();

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
		auto x = std::get<0>(InitIndexToCoord[index]);
		auto y = std::get<1>(InitIndexToCoord[index]);
		auto z = std::get<2>(InitIndexToCoord[index]);
		OccupationBoard[x][y][z] = index;
		LogicalBoard.push_back(std::make_tuple(x, y, z, domino, index));
		TilesMap[index] = domino;
	}

	std::sort(LogicalBoard.begin(), LogicalBoard.end(), Board::CompLogicalBoard);

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
		TilesMap[index + 140] = domino;
	}

	for(int index = 0; index < 140; ++index) Removable[0x0] = false;
	Removable[0x0] = true;
	Removable[0xb] = true;
	Removable[0xc] = true;
	Removable[0x13] = true;
	Removable[0x14] = true;
	Removable[0x1d] = true;
	Removable[0x36] = true;
	Removable[0x3f] = true;
	Removable[0x40] = true;
	Removable[0x47] = true;
	Removable[0x48] = true;
	Removable[0x53] = true;
	Removable[0x54] = true;
	Removable[0x59] = true;
	Removable[0x5a] = true;
	Removable[0x5f] = true;
	Removable[0x60] = true;
	Removable[0x65] = true;
	Removable[0x66] = true;
	Removable[0x6b] = true;
	Removable[0x6c] = true;
	Removable[0x71] = true;
	Removable[0x72] = true;
	Removable[0x77] = true;
	Removable[0x78] = true;
	Removable[0x7b] = true;
	Removable[0x7c] = true;
	Removable[0x7f] = true;
	Removable[0x80] = true;
	Removable[0x83] = true;
	Removable[0x84] = true;
	Removable[0x87] = true;
	Removable[0x8c] = true;
	Removable[0x8e] = true;
	Removable[0x8f] = true;

	Blocked = false;

	WhatsLeft.clear();
	for (int i = 0; i < 144; ++i) WhatsLeft.emplace_back(i);
}

void Board::RemoveTile(int index)
{
	TilesMap.erase(index);

	if (index > 139) Speciaux[index - 140] = -1;

	auto it = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (it != WhatsLeft.end())
		WhatsLeft.erase(it);

	Removable[index] = false;

	if (index == 0x8F)
	{
		Removable[0x88] = true;
		Removable[0x89] = true;
		Removable[0x8A] = true;
		Removable[0x8B] = true;
	}
	else if (index == 0x8C)
	{
		Removable[0x1E] = true;
		Removable[0x2A] = true;
	}
	else if (index == 0x8E)
	{
		Removable[0x8D] = true;
	}
	else if (index == 0x8D)
	{
		Removable[0x29] = true;
		Removable[0x35] = true;
	}
	else
	{
		std::vector<std::tuple<int, int, int, int, int>>::iterator it = LogicalBoard.begin();
		for (; it != LogicalBoard.end() && std::get<4>(*it) != index; ++it);
		int x = std::get<0>(*it);
		int y = std::get<1>(*it);
		int z = std::get<2>(*it);
		LogicalBoard.erase(it);

		OccupationBoard[x][y][z] = -1;
		if (x < 11 && OccupationBoard[x + 1][y][z] >= 0 && ( z > 3 || OccupationBoard[x + 1][y][z + 1] < 0 ))
			Removable[OccupationBoard[x + 1][y][z]] = true;
		if (x > 0 && OccupationBoard[x - 1][y][z] >= 0 && (z > 3 || OccupationBoard[x - 1][y][z + 1] < 0))
			Removable[OccupationBoard[x - 1][y][z]] = true;
		if (z > 0)
		{
			if (x < 11 && OccupationBoard[x + 1][y][z-1] < 0)
				Removable[OccupationBoard[x][y][z-1]] = true;
			if (x > 0 && OccupationBoard[x - 1][y][z-1] < 0)
				Removable[OccupationBoard[x][y][z-1]] = true;
		}
	}
	int max = 0;
	std::map<int, int> TotalMovesLeftByTile;
	for (std::map<int, int>::iterator it = TilesMap.begin(); it != TilesMap.end() && max < 2; ++it)
	{
		if (Removable[it->first])
		{
			auto tile = it->second;
			if (TotalMovesLeftByTile.contains(tile))
				TotalMovesLeftByTile[tile] += 1;
			else
				TotalMovesLeftByTile[tile] = 1;
			max = std::max(TotalMovesLeftByTile[tile], max);
		}
	}

	Blocked = max != 2;
}

void Board::RemovePairOfTiles(int first, int second)
{
	RemoveTile(first);
	RemoveTile(second);
}
