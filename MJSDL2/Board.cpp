#include "Board.h"
#include "Brute_force.h"

Board::Board()
{
}

bool Board::CompLogicalBoardDownLeft(const TileAndIndex& left, const TileAndIndex& right)
{
	return
		(
			arrIndexToCoord[left.Index].z <= arrIndexToCoord[right.Index].z &&
			(
				(arrIndexToCoord[left.Index].z != arrIndexToCoord[right.Index].z) ||
				(arrIndexToCoord[left.Index].y >= arrIndexToCoord[right.Index].y) && (arrIndexToCoord[left.Index].y != arrIndexToCoord[right.Index].y || arrIndexToCoord[left.Index].x <= arrIndexToCoord[right.Index].x)
				)
			);
	/*return !(std::get<2>(arrIndexToCoord[left.Index]) > std::get<2>(right) ||
		std::get<2>(arrIndexToCoord[left.Index]) == std::get<2>(right) &&
		(std::get<1>(arrIndexToCoord[left.Index]) < std::get<1>(right) || std::get<1>(arrIndexToCoord[left.Index]) == std::get<1>(right) && std::get<0>(arrIndexToCoord[left.Index]) > std::get<0>(right)));*/
}


bool Board::CompLogicalBoardDownRight(const TileAndIndex& left, const TileAndIndex& right)
{
	return
		(
			arrIndexToCoord[left.Index].z <= arrIndexToCoord[right.Index].z &&
			(
				(arrIndexToCoord[left.Index].z != arrIndexToCoord[right.Index].z) ||
				(arrIndexToCoord[left.Index].y >= arrIndexToCoord[right.Index].y) && (arrIndexToCoord[left.Index].y != arrIndexToCoord[right.Index].y || arrIndexToCoord[left.Index].x > arrIndexToCoord[right.Index].x)
				)
			);
}
bool Board::CompLogicalBoardUpLeft(const TileAndIndex& left, const TileAndIndex& right)
{
	return
		(
			arrIndexToCoord[left.Index].z <= arrIndexToCoord[right.Index].z &&
			(
				(arrIndexToCoord[left.Index].z != arrIndexToCoord[right.Index].z) ||
				(arrIndexToCoord[left.Index].y <= arrIndexToCoord[right.Index].y) && (arrIndexToCoord[left.Index].y != arrIndexToCoord[right.Index].y || arrIndexToCoord[left.Index].x <= arrIndexToCoord[right.Index].x)
				)
			);
}

bool Board::CompLogicalBoardUpRight(const TileAndIndex& left, const TileAndIndex& right)
{
	return
		(
			arrIndexToCoord[left.Index].z <= arrIndexToCoord[right.Index].z &&
			(
				(arrIndexToCoord[left.Index].z != arrIndexToCoord[right.Index].z) ||
				(arrIndexToCoord[left.Index].y <= arrIndexToCoord[right.Index].y) && (arrIndexToCoord[left.Index].y != arrIndexToCoord[right.Index].y || arrIndexToCoord[left.Index].x > arrIndexToCoord[right.Index].x)
				)
			);
}

void Board::SortBoard(const uint8_t direction)
{
	bool (*Comparateur)(const TileAndIndex&left, const TileAndIndex&right);
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
	std::sort(vLogicalBoard.begin(), vLogicalBoard.end(), Comparateur);
	std::vector< TileAndIndex>::iterator it;
	if (direction == 0 || direction == 3)
	{
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 140; ++it);
		if (it != vLogicalBoard.end())
		{
			auto temp = TileAndIndex(it->TileObject, it->Index);
			vLogicalBoard.erase(it);
			vLogicalBoard.insert(vLogicalBoard.begin(), temp);
		}
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 141; ++it);
		if (it != vLogicalBoard.end())
		{
			/*auto t = std::make_tuple(12, 8, 0, 0, 0);
			it = std::find_if(vLogicalBoard.begin(), vLogicalBoard.end(),
				[&t](const std::tuple<double, double, double, int, int>& in)
				{
					return (std::get<2>(t) == std::get<2>(in)) && (std::get<1>(t) == std::get<1>(in)) && (std::get<0>(t) == std::get<0>(in));
				}
			);*/
			auto temp = TileAndIndex(it->TileObject, it->Index);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 142; ++it);
		if (it != vLogicalBoard.end())
		{
			auto temp = TileAndIndex(it->TileObject, it->Index);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 143; ++it);
		if (it != vLogicalBoard.end())
		{
			auto temp = TileAndIndex(it->TileObject, it->Index);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
	}
	else if (direction == 1 || direction == 2)
		{
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 140; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index);
				vLogicalBoard.erase(it);
				vLogicalBoard.emplace_back(temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 141; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index);
				vLogicalBoard.erase(it);
				vLogicalBoard.insert(vLogicalBoard.begin(), temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 142; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index);
				vLogicalBoard.erase(it);
				vLogicalBoard.insert(vLogicalBoard.begin(), temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 143; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index);
				vLogicalBoard.erase(it);
				vLogicalBoard.emplace_back(temp);
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
	vLogicalBoard.clear();
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
		vLogicalBoard.emplace_back(TileAndIndex(domino, index));
		mIndexToTile.emplace(index, Tile(domino));
	}

	arrRemovable = InitRemovable();

	vWhatsLeft.clear();
	for (int i = 0; i < 144; ++i) vWhatsLeft.emplace_back(i);

	SetMoves();

#ifdef _DEBUG
	std::cout << std::dec << vWhatsLeft.size() << " tile" << (vWhatsLeft.size() > 1 ? "s" : "") << " left." << std::endl;
	std::cout << std::dec << vMoves.size() << " move" << (vMoves.size() > 1 ? "s" : "") << " left." << std::endl;
	auto it = vMoves.begin();
	if (it != vMoves.end())
	{
		std::cout << "(" << it->first << ";" << it->second << ")";
		for (++it; it != vMoves.end(); ++it)
			std::cout << ", (" << it->first << ";" << it->second << ")";
		std::cout << "." << std::endl;
	}
#endif
}

void Board::RemoveTile(const int index)
{
	mIndexToTile.erase(index);

	std::vector<TileAndIndex>::iterator it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end() && it->Index != index; ++it);
	auto coord = arrIndexToCoord[it->Index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;
	vLogicalBoard.erase(it);
	mOccupationBoard.erase(coord);

	auto itWL = std::find(vWhatsLeft.begin(), vWhatsLeft.end(), index);
	if (itWL != vWhatsLeft.end())
		vWhatsLeft.erase(itWL);

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
	if (left.Pairing == right.Pairing)
	{
		RemoveTile(first);
		RemoveTile(second);
		SetMoves();
		bRetour = true;
		vHistory.emplace_back(std::pair(first, second));
	}
	return bRetour;
}

void Board::BuildMoves(std::vector<TileAndIndex>& vRemovableBoard, std::vector<TileAndIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& vMoves)
{
	if (itFirst != vRemovableBoard.end())
	{
		auto domino = (*itFirst).TileObject;
		auto itNext = itFirst;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, vRemovableBoard.end(),
				[domino](const TileAndIndex& in)
				{
					return in.TileObject.Pairing == domino.Pairing;
				}
			);
			if (itNext != vRemovableBoard.end())
			{
				int indexFirst = (*itFirst).Index;
				vMoves.emplace_back(std::pair((*itFirst).Index, (*itNext).Index));
			}
		} while (itNext != vRemovableBoard.end());

		BuildMoves(vRemovableBoard, ++itFirst, vMoves);
	}
}

bool Board::CompRemovableBoard(const TileAndIndex& left, const TileAndIndex& right)
{
	return left.TileObject < right.TileObject;
}

void Board::SetMoves()
{
	std::vector<TileAndIndex> RemovableBoard; // (TileObject, Index)
	for (const auto& pair : vLogicalBoard)
	{
		if (arrRemovable[pair.Index]) RemovableBoard.emplace_back(pair);
	}

	vMoves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), Board::CompRemovableBoard);
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, vMoves);
}

bool Board::Solve()
{
	if (SolveRecInit(*this, vMoves, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution))
	{
#ifdef _DEBUG
		for (auto& move : vSolution)
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
