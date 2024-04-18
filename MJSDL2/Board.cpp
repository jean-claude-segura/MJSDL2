#include "Board.h"

Board::Board()
{
}

bool Board::CompLogicalBoardDownLeft(const std::pair<int, int>& left, const std::pair<int, int>& right)
{
	return
		(
			std::get<2>(IndexToCoord[left.second]) <= std::get<2>(IndexToCoord[right.second]) &&
			(
				(std::get<2>(IndexToCoord[left.second]) != std::get<2>(IndexToCoord[right.second])) ||
				(std::get<1>(IndexToCoord[left.second]) >= std::get<1>(IndexToCoord[right.second])) && (std::get<1>(IndexToCoord[left.second]) != std::get<1>(IndexToCoord[right.second]) || std::get<0>(IndexToCoord[left.second]) <= std::get<0>(IndexToCoord[right.second]))
				)
			);
	/*return !(std::get<2>(IndexToCoord[left.second]) > std::get<2>(right) ||
		std::get<2>(IndexToCoord[left.second]) == std::get<2>(right) &&
		(std::get<1>(IndexToCoord[left.second]) < std::get<1>(right) || std::get<1>(IndexToCoord[left.second]) == std::get<1>(right) && std::get<0>(IndexToCoord[left.second]) > std::get<0>(right)));*/
}


bool Board::CompLogicalBoardDownRight(const std::pair<int, int>& left, const std::pair<int, int>& right)
{
	return
		(
			std::get<2>(IndexToCoord[left.second]) <= std::get<2>(IndexToCoord[right.second]) &&
			(
				(std::get<2>(IndexToCoord[left.second]) != std::get<2>(IndexToCoord[right.second])) ||
				(std::get<1>(IndexToCoord[left.second]) >= std::get<1>(IndexToCoord[right.second])) && (std::get<1>(IndexToCoord[left.second]) != std::get<1>(IndexToCoord[right.second]) || std::get<0>(IndexToCoord[left.second]) > std::get<0>(IndexToCoord[right.second]))
				)
			);
}
bool Board::CompLogicalBoardUpLeft(const std::pair<int, int>& left, const std::pair<int, int>& right)
{
	return
		(
			std::get<2>(IndexToCoord[left.second]) <= std::get<2>(IndexToCoord[right.second]) &&
			(
				(std::get<2>(IndexToCoord[left.second]) != std::get<2>(IndexToCoord[right.second])) ||
				(std::get<1>(IndexToCoord[left.second]) <= std::get<1>(IndexToCoord[right.second])) && (std::get<1>(IndexToCoord[left.second]) != std::get<1>(IndexToCoord[right.second]) || std::get<0>(IndexToCoord[left.second]) <= std::get<0>(IndexToCoord[right.second]))
				)
			);
}

bool Board::CompLogicalBoardUpRight(const std::pair<int, int>& left, const std::pair<int, int>& right)
{
	return
		(
			std::get<2>(IndexToCoord[left.second]) <= std::get<2>(IndexToCoord[right.second]) &&
			(
				(std::get<2>(IndexToCoord[left.second]) != std::get<2>(IndexToCoord[right.second])) ||
				(std::get<1>(IndexToCoord[left.second]) <= std::get<1>(IndexToCoord[right.second])) && (std::get<1>(IndexToCoord[left.second]) != std::get<1>(IndexToCoord[right.second]) || std::get<0>(IndexToCoord[left.second]) > std::get<0>(IndexToCoord[right.second]))
				)
			);
}

void Board::SortBoard(const uint8_t direction)
{
	bool (*Comparateur)(const std::pair<int, int>&left, const std::pair<int, int>&right);
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
	std::vector< std::pair<int, int>>::iterator it;
	if (direction == 0 || direction == 3)
	{
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 140; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_pair(it->first, it->second);
			LogicalBoard.erase(it);
			LogicalBoard.insert(LogicalBoard.begin(), temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 141; ++it);
		if (it != LogicalBoard.end())
		{
			/*auto t = std::make_tuple(12, 8, 0, 0, 0);
			it = std::find_if(LogicalBoard.begin(), LogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);*/
			auto temp = std::make_pair(it->first, it->second);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 142; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_pair(it->first, it->second);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 143; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = std::make_pair(it->first, it->second);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
	}
	else if (direction == 1 || direction == 2)
		{
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 140; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_pair(it->first, it->second);
				LogicalBoard.erase(it);
				LogicalBoard.emplace_back(temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 141; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_pair(it->first, it->second);
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 142; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_pair(it->first, it->second);
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->second != 143; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = std::make_pair(it->first, it->second);
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
		mOccupationBoard[IndexToCoord[index]] = index;
		LogicalBoard.emplace_back(std::make_pair(domino, index));
		TilesMap[index] = domino;
	}

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

	std::vector<std::pair<int, int>>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && it->second != index; ++it);
	auto coord = IndexToCoord[it->second];
	double x = std::get<0>(coord);
	double y = std::get<1>(coord);
	double z = std::get<2>(coord);
	LogicalBoard.erase(it);
	mOccupationBoard.erase(coord);

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

void Board::BuildMoves(std::vector<std::pair<int, int>>& RemovableBoard, std::vector<std::pair<int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).first;
		auto itNext = itFirst;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, RemovableBoard.end(),
				[domino](const std::pair<int, int>& in)
				{
					return
						(
							in.first == domino ||
							(34 <= in.first && in.first < 38 && 34 <= domino && domino < 38) || // Saisons
							(38 <= in.first && in.first < 42 && 38 <= domino && domino < 42) // Fleurs.
							);
				}
			);
			if (itNext != RemovableBoard.end())
			{
				int indexFirst = (*itFirst).second;
				Moves.emplace_back(std::make_pair((*itFirst).second, (*itNext).second));
			}
		} while (itNext != RemovableBoard.end());

		BuildMoves(RemovableBoard, ++itFirst, Moves);
	}
}

bool Board::CompRemovableBoard(const std::pair<int, int>& left, const std::pair<int, int>& right)
{
	return left.first < right.first;
}

void Board::SetMoves()
{
	std::vector<std::pair<int, int>> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.second]) RemovableBoard.emplace_back(pair);
	}

	Moves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), Board::CompRemovableBoard);
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

bool Board::Solve()
{
	if (SolveRecInit(Moves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution))
	{
#ifdef _DEBUG
		for (auto& move : Solution)
			std::cout << move.first << ";" << move.second << std::endl;
#endif
		return true;
	}
	return false;
}
