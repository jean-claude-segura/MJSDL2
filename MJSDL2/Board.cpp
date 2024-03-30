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

	InitIndexToCoord[140] = std::make_tuple(-1, 3.5, 0);
	InitIndexToCoord[141] = std::make_tuple(12, 3.5, 0);
	InitIndexToCoord[142] = std::make_tuple(13, 3.5, 0);
	InitIndexToCoord[143] = std::make_tuple(5.5, 3.5, 4);

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
				mOccupationBoard[std::make_tuple<double, double, double>(x, y, z)] = -1;

	InitBoard();
}

bool Board::CompLogicalBoard(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	/*
	auto yFloor = ((std::floor(yLeft) >= std::floor(yRight)) && (std::floor(yLeft) != std::floor(yRight) || xLeft <= xRight)); // Domino droite
	auto yCeil = ((std::ceil(yLeft) >= std::ceil(yRight)) && (std::ceil(yLeft) != std::ceil(yRight) || xLeft <= xRight)); // Domino gauche
	auto yNormal = (yLeft >= yRight) && (yLeft != yRight || xLeft <= xRight); // Domino droite
	*/
	return
		(
			std::get<2>(left) <= std::get<2>(right) &&
			(
				(std::get<2>(left) != std::get<2>(right)) ||
				(std::get<1>(left) >= std::get<1>(right)) && (std::get<1>(left) != std::get<1>(right) || std::get<0>(left) <= std::get<0>(right))
			)
		);
	return ! (std::get<2>(left) > std::get<2>(right) ||
		std::get<2>(left) == std::get<2>(right) &&
		(std::get<1>(left) < std::get<1>(right) || std::get<1>(left) == std::get<1>(right) && std::get<0>(left) > std::get<0>(right)));
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
		auto m = std::make_tuple(x, y, z);
		mOccupationBoard[m] = index;
		LogicalBoard.push_back(std::make_tuple(x, y, z, domino, index));
		TilesMap[index] = domino;
	}

	std::sort(LogicalBoard.begin(), LogicalBoard.end(), Board::CompLogicalBoard);
	for (int index = 140; index < 144; ++index)
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
		auto m = std::make_tuple(x, y, z);
		mOccupationBoard[m] = index;
		std::vector<std::tuple<double, double, double, int, int>>::iterator it;
		switch (index)
		{
		case 140:
			it = LogicalBoard.begin();
			break;
		case 141:
		{
			auto t = std::make_tuple(12, 8, 0, 0, 0);
			it = std::find_if(LogicalBoard.begin(), LogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);
			break;
		}
		case 142:
		{
			auto t = std::make_tuple(13, 8, 0, 0, 0);
			it = std::find_if(LogicalBoard.begin(), LogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);
			break;
		}
		case 143:
			it = LogicalBoard.end();
			break;
		}
		LogicalBoard.insert(it, std::make_tuple(x, y, z, domino, index));
		TilesMap[index] = domino;
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

	WhatsLeft.clear();
	for (int i = 0; i < 144; ++i) WhatsLeft.emplace_back(i);
	SetMoves();
#ifdef _DEBUG
	std::cout << std::dec << WhatsLeft.size() << " tile" << (WhatsLeft.size() > 1 ? "s" : "") << " left." << std::endl;
	std::cout << std::dec << Moves.size() << " move" << (Moves.size() > 1 ? "s" : "") << " left." << std::endl;
	auto it = Moves.begin();
	if (it != Moves.end())
	{
		std::cout << "(" << it->first << ";" << it->second << ")";
		for (++it; it != Moves.end(); ++it)
			std::cout << ", (" << it->first << ";" << it->second << ")";
		std::cout << "." << std::endl;
	}
#endif
}

void Board::RemoveTile(const int index)
{
	TilesMap.erase(index);

	std::vector<std::tuple<double, double, double, int, int>>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && std::get<4>(*it) != index; ++it);
	int x = std::get<0>(*it);
	int y = std::get<1>(*it);
	int z = std::get<2>(*it);
	LogicalBoard.erase(it);
	mOccupationBoard[std::make_tuple<double, double, double>(x, y, z)] = -1;

	auto itWL = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (itWL != WhatsLeft.end())
		WhatsLeft.erase(itWL);

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
		if (x < 11 && mOccupationBoard[std::make_tuple<double, double, double>(x+1, y, z)] >= 0 && ( z > 3 || mOccupationBoard[std::make_tuple<double, double, double>(x+1, y, z+1)] < 0 ))
			Removable[mOccupationBoard[std::make_tuple<double, double, double>(x+1, y, z)]] = true;
		if (x > 0 && mOccupationBoard[std::make_tuple<double, double, double>(x-1, y, z)] >= 0 && (z > 3 || mOccupationBoard[std::make_tuple<double, double, double>(x-1, y, z+1)] < 0))
			Removable[mOccupationBoard[std::make_tuple<double, double, double>(x - 1, y, z)]] = true;
		if (z > 0)
		{
			if (x < 11 && mOccupationBoard[std::make_tuple<double, double, double>(x+1, y, z-1)] < 0)
				Removable[mOccupationBoard[std::make_tuple<double, double, double>(x, y, z-1)]] = true;
			if (x > 0 && mOccupationBoard[std::make_tuple<double, double, double>(x-1, y, z-1)] < 0)
				Removable[mOccupationBoard[std::make_tuple<double, double, double>(x, y, z-1)]] = true;
		}
	}
}

void Board::RemovePairOfTiles(const int first, const int second)
{
	RemoveTile(first);
	RemoveTile(second);
	SetMoves();
}

void Board::Tree(std::vector<std::tuple<double, double, double, int, int>>::iterator itNextIn, std::vector<std::pair<int, int>>& Moves, int domino)
{
	auto itFirst = std::find_if(itNextIn, RemovableBoard.end(),
		[domino](const std::tuple<double, double, double, int, int>& in)
		{
			return std::get<3>(in) == domino;
		}
	);
	if (itFirst != RemovableBoard.end())
	{
		int indexFirst = std::get<4>(*itFirst);
		auto itNext = itFirst;
		do
		{
			itNext = std::find_if(++itNext, RemovableBoard.end(),
				[domino](const std::tuple<double, double, double, int, int>& in)
				{
					return std::get<3>(in) == domino;
				}
			);
			if (itNext != RemovableBoard.end())
				Moves.emplace_back(std::make_pair(indexFirst, std::get<4>(*itNext)));
		} while (itNext != RemovableBoard.end());

		if (++itFirst != RemovableBoard.end())
			Tree(itFirst, Moves, domino);
	}
}

void Board::SetMoves()
{
	int max = 0;
	std::map<int, int> TotalMovesLeftByTile;
	for (std::map<int, int>::iterator it = TilesMap.begin(); it != TilesMap.end() /* && max < 2*/; ++it)
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


	RemovableBoard.clear();
	for (const auto& tuple : LogicalBoard)
	{
		if (Removable[std::get<4>(tuple)]) RemovableBoard.emplace_back(tuple);
	}

	Moves.clear();
	for (std::map<int, int>::iterator it = TotalMovesLeftByTile.begin(); it != TotalMovesLeftByTile.end(); ++it)
	{
		if (it->second > 1)
		{
			int domino = it->first;
			Tree(RemovableBoard.begin(), Moves, domino);
		}
	}
}
