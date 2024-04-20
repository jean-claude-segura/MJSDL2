#include "Board.h"
#include "Brute_force.h"

Board::Board()
{
}

bool Board::CompLogicalBoardDownLeft(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			IndexToCoord[left.index].z <= IndexToCoord[right.index].z &&
			(
				(IndexToCoord[left.index].z != IndexToCoord[right.index].z) ||
				(IndexToCoord[left.index].y >= IndexToCoord[right.index].y) && (IndexToCoord[left.index].y != IndexToCoord[right.index].y || IndexToCoord[left.index].x <= IndexToCoord[right.index].x)
				)
			);
	/*return !(std::get<2>(IndexToCoord[left.index]) > std::get<2>(right) ||
		std::get<2>(IndexToCoord[left.index]) == std::get<2>(right) &&
		(std::get<1>(IndexToCoord[left.index]) < std::get<1>(right) || std::get<1>(IndexToCoord[left.index]) == std::get<1>(right) && std::get<0>(IndexToCoord[left.index]) > std::get<0>(right)));*/
}


bool Board::CompLogicalBoardDownRight(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			IndexToCoord[left.index].z <= IndexToCoord[right.index].z &&
			(
				(IndexToCoord[left.index].z != IndexToCoord[right.index].z) ||
				(IndexToCoord[left.index].y >= IndexToCoord[right.index].y) && (IndexToCoord[left.index].y != IndexToCoord[right.index].y || IndexToCoord[left.index].x > IndexToCoord[right.index].x)
				)
			);
}
bool Board::CompLogicalBoardUpLeft(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			IndexToCoord[left.index].z <= IndexToCoord[right.index].z &&
			(
				(IndexToCoord[left.index].z != IndexToCoord[right.index].z) ||
				(IndexToCoord[left.index].y <= IndexToCoord[right.index].y) && (IndexToCoord[left.index].y != IndexToCoord[right.index].y || IndexToCoord[left.index].x <= IndexToCoord[right.index].x)
				)
			);
}

bool Board::CompLogicalBoardUpRight(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			IndexToCoord[left.index].z <= IndexToCoord[right.index].z &&
			(
				(IndexToCoord[left.index].z != IndexToCoord[right.index].z) ||
				(IndexToCoord[left.index].y <= IndexToCoord[right.index].y) && (IndexToCoord[left.index].y != IndexToCoord[right.index].y || IndexToCoord[left.index].x > IndexToCoord[right.index].x)
				)
			);
}

void Board::SortBoard(const uint8_t direction)
{
	bool (*Comparateur)(const DominoIndex&left, const DominoIndex&right);
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
	std::vector< DominoIndex>::iterator it;
	if (direction == 0 || direction == 3)
	{
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 140; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = DominoIndex(it->domino, it->index);
			LogicalBoard.erase(it);
			LogicalBoard.insert(LogicalBoard.begin(), temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 141; ++it);
		if (it != LogicalBoard.end())
		{
			/*auto t = std::make_tuple(12, 8, 0, 0, 0);
			it = std::find_if(LogicalBoard.begin(), LogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);*/
			auto temp = DominoIndex(it->domino, it->index);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 142; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = DominoIndex(it->domino, it->index);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
		for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 143; ++it);
		if (it != LogicalBoard.end())
		{
			auto temp = DominoIndex(it->domino, it->index);
			LogicalBoard.erase(it);
			LogicalBoard.emplace_back(temp);
		}
	}
	else if (direction == 1 || direction == 2)
		{
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 140; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = DominoIndex(it->domino, it->index);
				LogicalBoard.erase(it);
				LogicalBoard.emplace_back(temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 141; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = DominoIndex(it->domino, it->index);
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 142; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = DominoIndex(it->domino, it->index);
				LogicalBoard.erase(it);
				LogicalBoard.insert(LogicalBoard.begin(), temp);
			}
			for (it = LogicalBoard.begin(); it != LogicalBoard.end() && it->index != 143; ++it);
			if (it != LogicalBoard.end())
			{
				auto temp = DominoIndex(it->domino, it->index);
				LogicalBoard.erase(it);
				LogicalBoard.emplace_back(temp);
			}
		}
}

void Board::InitBoard()
{
#ifdef _DEBUG
	// 1, 5, 7, 12, 18, 19, 24, 25, 27, 31, 34, 36, 44, 49, 50, 55, 61, 62, 64, 66
	static int seed = 0;
	std::mt19937 e1(seed++);
	std::cout << "******************************************* " << seed - 1 << " *******************************************" << std::endl;
#else
	std::random_device r;
	std::default_random_engine e1(r());
#endif
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
		LogicalBoard.emplace_back(DominoIndex(domino, index));
		TilesMap.emplace(index, Domino(domino));
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

	std::vector<DominoIndex>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && it->index != index; ++it);
	auto coord = IndexToCoord[it->index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;
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
		if (x < 11 && mOccupationBoard.contains(Coordinates(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x + 1, y, z + 1))))
			Removable[mOccupationBoard[Coordinates(x + 1, y, z)]] = true;
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
			Removable[mOccupationBoard[Coordinates(x - 1, y, z)]] = true;
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
				Removable[mOccupationBoard[Coordinates(x, y, z - 1)]] = true;
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
				Removable[mOccupationBoard[Coordinates(x, y, z - 1)]] = true;
		}
	}
}

bool Board::RemovePairOfTiles(const int first, const int second)
{
	bool bRetour = false;
	auto left = TilesMap.find(first)->second;
	auto right = TilesMap.find(second)->second;
	if (left.appairage == right.appairage)
	{
		RemoveTile(first);
		RemoveTile(second);
		SetMoves();
		bRetour = true;
		History.emplace_back(std::pair(first, second));
	}
	return bRetour;
}

void Board::BuildMoves(std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).domino;
		auto itNext = itFirst;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, RemovableBoard.end(),
				[domino](const DominoIndex& in)
				{
					return in.domino.appairage == domino.appairage;
				}
			);
			if (itNext != RemovableBoard.end())
			{
				int indexFirst = (*itFirst).index;
				Moves.emplace_back(std::pair((*itFirst).index, (*itNext).index));
			}
		} while (itNext != RemovableBoard.end());

		BuildMoves(RemovableBoard, ++itFirst, Moves);
	}
}

bool Board::CompRemovableBoard(const DominoIndex& left, const DominoIndex& right)
{
	return left.domino < right.domino;
}

void Board::SetMoves()
{
	std::vector<DominoIndex> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	Moves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), Board::CompRemovableBoard);
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

bool Board::Solve()
{
	if (SolveRecInit(*this, Moves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution))
	{
#ifdef _DEBUG
		for (auto& move : Solution)
			std::cout << move.first << ";" << move.second << std::endl;
#endif
		return true;
	}
	return false;
}
