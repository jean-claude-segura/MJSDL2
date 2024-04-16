#include "Board.h"

Board::Board()
{
	std::array<std::array<std::array<bool, 5>, 8>, 12> BasePattern;
	for (int z = 0; z < 5; ++z)
		for (int y = 0; y < 8; ++y)
			for (int x = 0; x < 12; ++x)
				BasePattern[x][y][z] = false;
	for (int x = 0; x < 12; ++x)
	{
		BasePattern[x][0][0] = true;
		BasePattern[x][7][0] = true;
		BasePattern[x][3][0] = true;
		BasePattern[x][4][0] = true;
	}

	for (int x = 2; x < 10; ++x)
	{
		BasePattern[x][1][0] = true;
		BasePattern[x][6][0] = true;
	}
	for (int x = 1; x < 11; ++x)
	{
		BasePattern[x][2][0] = true;
		BasePattern[x][5][0] = true;
	}
	for (int x = 3; x < 9; ++x)
		for (int y = 1; y < 7; ++y)
			BasePattern[x][y][1] = true;

	for (int x = 4; x < 8; ++x)
		for (int y = 2; y < 6; ++y)
			BasePattern[x][y][2] = true;

	for (int x = 5; x < 7; ++x)
		for (int y = 3; y < 5; ++y)
			BasePattern[x][y][3] = true;

	int index = 0;
	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) InitIndexToCoord[index++] = std::make_tuple(x, y, z);
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
}

bool Board::CompLogicalBoardDownLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	return
		(
			std::get<2>(left) <= std::get<2>(right) &&
			(
				(std::get<2>(left) != std::get<2>(right)) ||
				(std::get<1>(left) >= std::get<1>(right)) && (std::get<1>(left) != std::get<1>(right) || std::get<0>(left) <= std::get<0>(right))
				)
			);
	/*return !(std::get<2>(left) > std::get<2>(right) ||
		std::get<2>(left) == std::get<2>(right) &&
		(std::get<1>(left) < std::get<1>(right) || std::get<1>(left) == std::get<1>(right) && std::get<0>(left) > std::get<0>(right)));*/
}


bool Board::CompLogicalBoardDownRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	return
		(
			std::get<2>(left) <= std::get<2>(right) &&
			(
				(std::get<2>(left) != std::get<2>(right)) ||
				(std::get<1>(left) >= std::get<1>(right)) && (std::get<1>(left) != std::get<1>(right) || std::get<0>(left) > std::get<0>(right))
				)
			);
}
bool Board::CompLogicalBoardUpLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	return
		(
			std::get<2>(left) <= std::get<2>(right) &&
			(
				(std::get<2>(left) != std::get<2>(right)) ||
				(std::get<1>(left) <= std::get<1>(right)) && (std::get<1>(left) != std::get<1>(right) || std::get<0>(left) <= std::get<0>(right))
				)
			);
}

bool Board::CompLogicalBoardUpRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	return
		(
			std::get<2>(left) <= std::get<2>(right) &&
			(
				(std::get<2>(left) != std::get<2>(right)) ||
				(std::get<1>(left) <= std::get<1>(right)) && (std::get<1>(left) != std::get<1>(right) || std::get<0>(left) > std::get<0>(right))
				)
			);
}

void Board::SortBoard(const uint8_t direction)
{
	bool (*Comparateur)(const std::tuple<double, double, double, int, int>&left, const std::tuple<double, double, double, int, int>&right);
	switch (direction)
	{
	default:
	case 3:
		Comparateur = Board::CompLogicalBoardDownLeft;
		break;
	case 0:
		Comparateur = Board::CompLogicalBoardUpLeft;
		break;
	case 1:
		Comparateur = Board::CompLogicalBoardUpRight;
		break;
	case 2:
		Comparateur = Board::CompLogicalBoardDownRight;
		break;
	}
	std::sort(LogicalBoard.begin(), LogicalBoard.end(), Comparateur);
	std::vector< std::tuple<double, double, double, int, int>>::iterator it;
	if (direction == 0 || direction == 3)
	{
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 140; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
			LogicalBoard.erase(it);
			LogicalBoard.insert(LogicalBoard.begin(), temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 141; ++it);
		if (it != LogicalBoard.end())
		{
			/*auto t = std::make_tuple(12, 8, 0, 0, 0);
			it = std::find_if(LogicalBoard.begin(), LogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);*/
			auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 142; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 143; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
	}
	else if (direction == 1 || direction == 2)
		{
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 140; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
				LogicalBoard.erase(it);
				LogicalBoard.emplace_back(temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 141; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 142; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && std::get<4>(*it) != 143; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_tuple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), std::get<3>(*it), std::get<4>(*it));
				LogicalBoard.erase(it);
				LogicalBoard.emplace_back(temp);
			}
		}
}

void Board::InitBoard()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, 41);

	LogicalBoard.clear();
	TilesMap.clear();
	mOccupationBoard.clear();

	int tempDominos[42] = {
	4,4,4,4,4,4,4,4,4, // Bambous
	4,4,4,4,4,4,4,4,4, // Cercles
	4,4,4,4,4,4,4,4,4, // Caractères
	4,4,4,4, // Vents
	4,4,4, // Dragons
	1,1,1,1, // Saisons
	1,1,1,1 // Fleurs
	};

	for (int index = 0; index < 144; ++index)
	{
		int domino = 0;
		do
		{
			domino = uniform_dist(e1);
		} while (tempDominos[domino] == 0);
		int debugdom = tempDominos[domino];
		--tempDominos[domino];
		mOccupationBoard[InitIndexToCoord[index]] = index;
		auto x = std::get<0>(InitIndexToCoord[index]);
		auto y = std::get<1>(InitIndexToCoord[index]);
		auto z = std::get<2>(InitIndexToCoord[index]);
		LogicalBoard.emplace_back(std::make_tuple(x, y, z, domino, index));
		TilesMap[index] = domino;
	}

	//SortBoard(true);

	for(int index = 0; index < 144; ++index) Removable[index] = false;
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
	double x = std::get<0>(*it);
	double y = std::get<1>(*it);
	double z = std::get<2>(*it);
	LogicalBoard.erase(it);
	mOccupationBoard.erase(std::make_tuple(x, y, z));

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
		if (x < 11 && mOccupationBoard.contains(std::make_tuple(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x + 1, y, z + 1))))
			Removable[mOccupationBoard[std::make_tuple(x + 1, y, z)]] = true;
		if (x > 0 && mOccupationBoard.contains(std::make_tuple(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x - 1, y, z + 1))))
			Removable[mOccupationBoard[std::make_tuple(x - 1, y, z)]] = true;
		if (z > 0) // mOccupationBoard[std::make_tuple(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(std::make_tuple(x + 1, y, z - 1)))
				Removable[mOccupationBoard[std::make_tuple(x, y, z - 1)]] = true;
			if (x > 0 && !mOccupationBoard.contains(std::make_tuple(x - 1, y, z - 1)))
				Removable[mOccupationBoard[std::make_tuple(x, y, z - 1)]] = true;
		}
	}
}

bool Board::RemovePairOfTiles(const int first, const int second)
{
	bool bRetour = false;
	auto left = TilesMap[first];
	auto right = TilesMap[second];
	if (
		left == right ||
		(34 <= left && left < 38 && 34 <= right && right < 38) || // Saisons
		(38 <= left && left < 42 && 38 <= right && right < 42) // Fleurs.
		)
	{
		RemoveTile(first);
		RemoveTile(second);
		SetMoves();
		bRetour = true;
	}
	return bRetour;
}

void Board::BuildMoves(std::vector<std::tuple<double, double, double, int, int>>& RemovableBoard, std::vector<std::tuple<double, double, double, int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = std::get<3>(*itFirst);
		auto itNext = itFirst;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, RemovableBoard.end(),
				[domino](const std::tuple<double, double, double, int, int>& in)
				{
					return
						(
							std::get<3>(in) == domino ||
							(34 <= std::get<3>(in) && std::get<3>(in) < 38 && 34 <= domino && domino < 38) || // Saisons
							(38 <= std::get<3>(in) && std::get<3>(in) < 42 && 38 <= domino && domino < 42) // Fleurs.
							);
				}
			);
			if (itNext != RemovableBoard.end())
			{
				int indexFirst = std::get<4>(*itFirst);
				Moves.emplace_back(std::make_pair(std::get<4>(*itFirst), std::get<4>(*itNext)));
			}
		} while (itNext != RemovableBoard.end());

		BuildMoves(RemovableBoard, ++itFirst, Moves);
	}
}

bool Board::CompRemovableBoard(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
{
	return std::get<3>(left) < std::get<3>(right);
}

void Board::SetMoves()
{
	std::vector<std::tuple<double, double, double, int, int>> RemovableBoard; // (x, y, z, domino, index)
	for (const auto& tuple : LogicalBoard)
	{
		if (Removable[std::get<4>(tuple)]) RemovableBoard.emplace_back(tuple);
	}

	Moves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), CompRemovableBoard);
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, Moves);
}
