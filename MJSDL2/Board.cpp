#include "Board.h"
#include "Brute_force.h"

Board::Board()
{
}

bool Board::CompLogicalBoardDownLeft(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			arrIndexToCoord[left.index].z <= arrIndexToCoord[right.index].z &&
			(
				(arrIndexToCoord[left.index].z != arrIndexToCoord[right.index].z) ||
				(arrIndexToCoord[left.index].y >= arrIndexToCoord[right.index].y) && (arrIndexToCoord[left.index].y != arrIndexToCoord[right.index].y || arrIndexToCoord[left.index].x <= arrIndexToCoord[right.index].x)
				)
			);
	/*return !(std::get<2>(arrIndexToCoord[left.index]) > std::get<2>(right) ||
		std::get<2>(arrIndexToCoord[left.index]) == std::get<2>(right) &&
		(std::get<1>(arrIndexToCoord[left.index]) < std::get<1>(right) || std::get<1>(arrIndexToCoord[left.index]) == std::get<1>(right) && std::get<0>(arrIndexToCoord[left.index]) > std::get<0>(right)));*/
}


bool Board::CompLogicalBoardDownRight(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			arrIndexToCoord[left.index].z <= arrIndexToCoord[right.index].z &&
			(
				(arrIndexToCoord[left.index].z != arrIndexToCoord[right.index].z) ||
				(arrIndexToCoord[left.index].y >= arrIndexToCoord[right.index].y) && (arrIndexToCoord[left.index].y != arrIndexToCoord[right.index].y || arrIndexToCoord[left.index].x > arrIndexToCoord[right.index].x)
				)
			);
}
bool Board::CompLogicalBoardUpLeft(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			arrIndexToCoord[left.index].z <= arrIndexToCoord[right.index].z &&
			(
				(arrIndexToCoord[left.index].z != arrIndexToCoord[right.index].z) ||
				(arrIndexToCoord[left.index].y <= arrIndexToCoord[right.index].y) && (arrIndexToCoord[left.index].y != arrIndexToCoord[right.index].y || arrIndexToCoord[left.index].x <= arrIndexToCoord[right.index].x)
				)
			);
}

bool Board::CompLogicalBoardUpRight(const DominoIndex& left, const DominoIndex& right)
{
	return
		(
			arrIndexToCoord[left.index].z <= arrIndexToCoord[right.index].z &&
			(
				(arrIndexToCoord[left.index].z != arrIndexToCoord[right.index].z) ||
				(arrIndexToCoord[left.index].y <= arrIndexToCoord[right.index].y) && (arrIndexToCoord[left.index].y != arrIndexToCoord[right.index].y || arrIndexToCoord[left.index].x > arrIndexToCoord[right.index].x)
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
	// 0, 1, 2, 5, 7, 12, 18, 19, 24, 25, 27, 31, 34, 36, 44, 49, 50, 55, 61, 62, 64, 66
	// [0, 2, 21, 32]
	static int seed = 0;
	std::mt19937 e1(seed++);
	std::cout << "******************************************* " << seed - 1 << " *******************************************" << std::endl;
#else
	std::random_device r;
	std::default_random_engine e1(r());
#endif
	std::uniform_int_distribution<int> uniform_dist(0, 41);
	LogicalBoard.clear();
	mIndexToTile.clear();
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
		mOccupationBoard[arrIndexToCoord[index]] = index;
		LogicalBoard.emplace_back(DominoIndex(domino, index));
		mIndexToTile.emplace(index, Domino(domino));
	}

	arrRemovable = InitRemovable();

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
	mIndexToTile.erase(index);

	std::vector<DominoIndex>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && it->index != index; ++it);
	auto coord = arrIndexToCoord[it->index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;
	LogicalBoard.erase(it);
	mOccupationBoard.erase(coord);

	auto itWL = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (itWL != WhatsLeft.end())
		WhatsLeft.erase(itWL);

	arrRemovable[index] = false;

	if (index == 0x8F)
	{
		arrRemovable[0x88] = true;
		arrRemovable[0x89] = true;
		arrRemovable[0x8A] = true;
		arrRemovable[0x8B] = true;
	}
	else if (index == 0x8C)
	{
		arrRemovable[0x1E] = true;
		arrRemovable[0x2A] = true;
	}
	else if (index == 0x8E)
	{
		arrRemovable[0x8D] = true;
	}
	else if (index == 0x8D)
	{
		arrRemovable[0x29] = true;
		arrRemovable[0x35] = true;
	}
	else
	{
		if (x < 11 && mOccupationBoard.contains(Coordinates(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x + 1, y, z + 1))))
			arrRemovable[mOccupationBoard[Coordinates(x + 1, y, z)]] = true;
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
			arrRemovable[mOccupationBoard[Coordinates(x - 1, y, z)]] = true;
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
				arrRemovable[mOccupationBoard[Coordinates(x, y, z - 1)]] = true;
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
				arrRemovable[mOccupationBoard[Coordinates(x, y, z - 1)]] = true;
		}
	}
}

bool Board::RemovePairOfTiles(const int first, const int second)
{
	bool bRetour = false;
	auto left = mIndexToTile.find(first)->second;
	auto right = mIndexToTile.find(second)->second;
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
		if (arrRemovable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	Moves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), Board::CompRemovableBoard);
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

bool Board::Solve()
{
	if (SolveRecInit(*this, Moves, LogicalBoard, arrRemovable, mIndexToTile, WhatsLeft, mOccupationBoard, Solution))
	{
#ifdef _DEBUG
		for (auto& move : Solution)
			std::cout << move.first << ";" << move.second << std::endl;
#endif
		return true;
	}
	return false;
}

#ifdef _DEBUG
bool Board::Test()
{
	uint64_t ret = 0ULL;
	int solved = 0;
	std::stringstream strout;
	std::vector<int> Locked;
	int i = 0;
	bool goOn = true;
	do
	{
		InitBoard();
		if (CheckIfLockedFromStart(mIndexToTile))
		{
			Locked.emplace_back(i++);
			continue;
		}
		auto temp = testAll(*this);
		if (temp != 0ULL)
			++solved;
		std::cout << std::dec << i << " : " << std::hex << temp << std::dec << std::endl;
		ret += temp;
		auto v9 = temp & 0b01111111;
		temp = temp >> 7;
		auto v8 = temp & 0b01111111;
		temp = temp >> 7;
		auto v7 = temp & 0b01111111;
		temp = temp >> 7;
		auto v6 = temp & 0b01111111;
		temp = temp >> 7;
		auto v5 = temp & 0b01111111;
		temp = temp >> 7;
		auto v4 = temp & 0b01111111;
		temp = temp >> 7;
		auto v3 = temp & 0b01111111;
		temp = temp >> 7;
		auto v2 = temp & 0b01111111;
		temp = temp >> 7;
		auto v1 = temp & 0b01111111;
		temp = temp >> 7;

		strout << std::endl;
		strout << std::dec << v1 << ", ";
		strout << std::dec << v2 << ", ";
		strout << std::dec << v3 << ", ";
		strout << std::dec << v4 << ", ";
		strout << std::dec << v5 << ", ";
		strout << std::dec << v6 << ", ";
		strout << std::dec << v7 << ", ";
		strout << std::dec << v8 << ", ";
		strout << std::dec << v9 << std::endl;
		++i;

		auto tempRet = ret;
		for (int dec = 0; dec < 9 && goOn; ++dec)
		{
			auto rv1 = tempRet & 0b01111111;
			tempRet = tempRet >> 7;
			if (rv1 == 0b01111111)
				goOn = false;
		}
	} while (goOn);

	std::cout << strout.str();

	auto v9 = ret & 0b01111111;
	ret = ret >> 7;
	auto v8 = ret & 0b01111111;
	ret = ret >> 7;
	auto v7 = ret & 0b01111111;
	ret = ret >> 7;
	auto v6 = ret & 0b01111111;
	ret = ret >> 7;
	auto v5 = ret & 0b01111111;
	ret = ret >> 7;
	auto v4 = ret & 0b01111111;
	ret = ret >> 7;
	auto v3 = ret & 0b01111111;
	ret = ret >> 7;
	auto v2 = ret & 0b01111111;
	ret = ret >> 7;
	auto v1 = ret & 0b01111111;
	ret = ret >> 7;

	std::cout << std::endl;
	std::cout << std::dec << v1 << ", ";
	std::cout << std::dec << v2 << ", ";
	std::cout << std::dec << v3 << ", ";
	std::cout << std::dec << v4 << ", ";
	std::cout << std::dec << v5 << ", ";
	std::cout << std::dec << v6 << ", ";
	std::cout << std::dec << v7 << ", ";
	std::cout << std::dec << v8 << ", ";
	std::cout << std::dec << v9 << std::endl;

	auto pourcent = solved * 100. / (i - Locked.size());

	std::cout << "Résolus : " << std::dec << pourcent << "%" << std::endl;

	std::cout << std::endl;

	if (!Locked.empty())
	{
		auto it = Locked.begin();
		for (; it != Locked.end() - 1; ++it)
		{
			std::cout << std::dec << *it << ", ";
		}
		std::cout << std::dec << *it << std::endl;
	}
	std::cout << "Bloqués : " << std::dec << Locked.size() << "." << std::endl;

	return false;
}

bool Board::TestLocked()
{
	std::vector<int> Locked;
	for (int i = 0; i < 99; ++i)
	{
		InitBoard();
		if (CheckIfLockedFromStart(mIndexToTile))
			Locked.emplace_back(i);
	}

	std::cout << std::endl;

	auto it = Locked.begin();
	for (; it != Locked.end() - 1; ++it)
	{
		std::cout << std::dec << *it << ", ";
	}
	std::cout << std::dec << *it << std::endl;

	std::cout << "Bloqués : " << std::dec << Locked.size() << "." << std::endl;

	return false;
}
#endif
