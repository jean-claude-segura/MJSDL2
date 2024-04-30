#include "Board.h"
#include "Brute_force.h"

Board::Board()
{
	Seed = 0;
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
			auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
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
			auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 142; ++it);
		if (it != vLogicalBoard.end())
		{
			auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
		for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 143; ++it);
		if (it != vLogicalBoard.end())
		{
			auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
			vLogicalBoard.erase(it);
			vLogicalBoard.emplace_back(temp);
		}
	}
	else if (direction == 1 || direction == 2)
		{
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 140; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
				vLogicalBoard.erase(it);
				vLogicalBoard.emplace_back(temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 141; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
				vLogicalBoard.erase(it);
				vLogicalBoard.insert(vLogicalBoard.begin(), temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 142; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
				vLogicalBoard.erase(it);
				vLogicalBoard.insert(vLogicalBoard.begin(), temp);
			}
			for (it = vLogicalBoard.begin(); it != vLogicalBoard.end() && it->Index != 143; ++it);
			if (it != vLogicalBoard.end())
			{
				auto temp = TileAndIndex(it->TileObject, it->Index, it->X, it->Y, it->Z, it->DecX, it->DecY);
				vLogicalBoard.erase(it);
				vLogicalBoard.emplace_back(temp);
			}
		}
}

std::array<bool, 144> Board::InitRemovable()
{
	for (int i = 0; i < 144; ++i)
		arrRemovable[i] = false;

	std::array<std::array<std::array<Tile, 4>, 8>, 12> arrBoard = {};

	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index < 140)
			arrBoard[tileAndIndex.X][tileAndIndex.Y][tileAndIndex.Z] = tileAndIndex.TileObject;
		else
			arrRemovable[tileAndIndex.Index] = true;
	}

	bool unlockPadlock = true;
	// I want to be able to sort anytime so two loops.
	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index < 140)
		{
			const auto x = tileAndIndex.X;
			const auto y = tileAndIndex.Y;
			const auto z = tileAndIndex.Z;
			if ((z == 3 || arrBoard[x][y][z + 1].Pairing == -1) && (x == arrHorizontalLimits[y][z].first || x == arrHorizontalLimits[y][z].second || arrBoard[x - 1][y][z].Pairing == -1 || arrBoard[x + 1][y][z].Pairing == -1))
				arrRemovable[tileAndIndex.Index] = true;

			if (tileAndIndex.Index == 0x29 || tileAndIndex.Index == 0x35) unlockPadlock = false;
		}
	}

	// I want to be able to sort anytime so two loops.
	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index >= 140)
		{
			if (tileAndIndex.Index == 0x8F)
			{
				arrRemovable[0x88] = false;
				arrRemovable[0x89] = false;
				arrRemovable[0x8A] = false;
				arrRemovable[0x8B] = false;
			}
			else if (tileAndIndex.Index == 0x8C)
			{
				arrRemovable[0x1E] = false;
				arrRemovable[0x2A] = false;
			}
			else if (tileAndIndex.Index == 0x8E)
			{
				arrRemovable[0x8D] = unlockPadlock;
			}
			else if (tileAndIndex.Index == 0x8D)
			{
				arrRemovable[0x29] = false;
				arrRemovable[0x35] = false;
			}
		}
	}

	return arrRemovable;
}

bool Board::Load(std::pair<std::string, std::vector<std::string>>& savedGame)
{
	Seed = std::stoul(savedGame.first);
	std::mt19937 e1(Seed);

	InitBoardSub(e1);

	for (auto it = savedGame.second.begin(); it != savedGame.second.end();)
	{
		RemoveTile(std::stoul(*it));
		RemoveTile(std::stoul(*(it + 1)));

		vHistory.emplace_back(std::make_pair(std::stoul(*it), std::stoul(*(it + 1))));

		IsLockedFromMove();

		++it;
		++it;
	}

	SetMoves();

	return true;
};

bool Board::Save(std::pair<std::string, std::vector<std::string>>& savedGame)
{
	savedGame.first = std::to_string(Seed);
	for (const auto& move : vHistory)
	{
		savedGame.second.emplace_back(std::to_string(move.first));
		savedGame.second.emplace_back(std::to_string(move.second));
	}
	return true;
};

void Board::InitBoardSub(std::mt19937 &e1)
{
	std::uniform_int_distribution<int> uniform_dist(0, 41);

	vLogicalBoard.clear();
	mIndexToTile.clear();
	mIndexToRemovedTile.clear();
	mOccupationBoard.clear();
	vHistory.clear();
	vForwardHistory.clear();
	vSolution.clear();
	bIsLockedFromMove = false;

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
#ifdef _DEBUG
		int debugdom = tempDominos[domino];
#endif
		--tempDominos[domino];
		mOccupationBoard[arrIndexToCoord[index]] = index;
		const auto& coord = arrIndexToBoardCoord[index];
		vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
		mIndexToTile.emplace(index, Tile(domino));
	}

	InitRemovable();

	SetMoves();

	bIsLockedFromStart = CheckIfLockedFromStart(vLogicalBoard, mIndexToTile);

#ifdef _DEBUG
#ifndef BENCHMARK
	std::cout << std::dec << vLogicalBoard.size() << " tile" << (vLogicalBoard.size() > 1 ? "s" : "") << " left." << std::endl;
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
#endif
}

void Board::InitBoard()
{
#ifdef _DEBUG
	// 0, 1, 2, 5, 7, 12, 18, 19, 24, 25, 27, 31, 34, 36, 44, 49, 50, 55, 61, 62, 64, 66
	// [0, 2, 21, 32]
	// 9 is interesting.
	static unsigned int seed = 0;
	Seed = seed;
	std::mt19937 e1(Seed);
	++seed;
	//std::mt19937 e1(1);
	std::cout << "******************************************* " << Seed << " *******************************************" << std::endl;
#else
	std::random_device r;
	std::mt19937 e1(r());
	// Création du seed.
	Seed = e1();
	// Affectation du seed.
	e1.seed(Seed);
#endif

	InitBoardSub(e1);
}

void Board::RemoveTile(const int index)
{
	mIndexToRemovedTile.emplace(index, mIndexToTile.find(index)->second);
	mIndexToTile.erase(index);

	std::vector<TileAndIndex>::iterator it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end() && it->Index != index; ++it);
	const auto & coord = arrIndexToCoord[it->Index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;
	vLogicalBoard.erase(it);
	mOccupationBoard.erase(coord);

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
	const auto & left = mIndexToTile.find(first)->second;
	const auto & right = mIndexToTile.find(second)->second;
	if (left.Pairing == right.Pairing)
	{
		RemoveTile(first);
		RemoveTile(second);
		SetMoves();
		vHistory.emplace_back(std::pair(first, second));
		vForwardHistory.clear();
		bRetour = true;
	}
	return bRetour;
}

void Board::BuildMoves(std::vector<TileAndIndex>& vRemovableBoard, std::vector<TileAndIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& vMoves)
{
	if (itFirst != vRemovableBoard.end())
	{
		const auto & domino = (*itFirst).TileObject;
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
	if (TryHeuristics(*this, vLogicalBoard, mIndexToTile, vSolution))
	{
#ifdef _DEBUG
		for (auto& move : vSolution)
			std::cout << move.first << ";" << move.second << std::endl;
#endif
		return true;
	}
	return false;
}

void Board::ComputerStop()
{
	if (solver.joinable())
	{
		stopSolverNow = true;
		solver.join();
#ifdef _DEBUG
		std::cout << "Computer stopped." << std::endl;
#endif
	}
}

bool Board::ComputerSolve()
{
	if (!solver.joinable())
	{
#ifdef _DEBUG
		uint64_t positions = 0ULL;
#endif
		stopSolverNow = false;
		solver = std::thread(&SolveRecAsyncInit, vMoves, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, std::ref(vSolution)
#ifdef _DEBUG
			, positions
#endif
		);
#ifdef _DEBUG
		std::cout << "Computer started." << std::endl;
#endif
		return true;
	}
	else
	{
		ComputerStop();
		return false;
	}
}

bool Board::IsLockedFromStart()
{
	return bIsLockedFromStart;
}

bool Board::IsLockedFromMove()
{
	if (vHistory.empty())
	{
		return false;
	}
	else
	{
		std::vector<int> vMove;
		const auto & it = vHistory.back();
		vMove.emplace_back(it.first);
		vMove.emplace_back(it.second);
		
		bIsLockedFromMove = bIsLockedFromMove ? true : CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, vMove);
		return bIsLockedFromMove;
	}
}

bool Board::TakeBack(bool beginning)
{
	if (vHistory.empty())
	{
		return false;
	}
	else
	{
		const auto & it = vHistory.back();
		if (!beginning)
			vForwardHistory.push_back(it);

		for(auto it = vHistory.rbegin(); it != vHistory.rend(); ++it)
		{
			if (beginning)
				vForwardHistory.push_back(*it);

			const auto firstIndex = it->first;
			const auto secondIndex = it->second;

			const auto & firstTile = mIndexToRemovedTile.find(firstIndex)->second;
			const auto & secondTile = mIndexToRemovedTile.find(secondIndex)->second;

			auto coord = arrIndexToBoardCoord[firstIndex];
			vLogicalBoard.emplace_back(TileAndIndex(firstTile.Rank, firstIndex, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));

			coord = arrIndexToBoardCoord[secondIndex];
			vLogicalBoard.emplace_back(TileAndIndex(secondTile.Rank, secondIndex, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));

			mIndexToTile.emplace(firstIndex, firstTile);
			mIndexToRemovedTile.erase(firstIndex);

			mIndexToTile.emplace(secondIndex, secondTile);
			mIndexToRemovedTile.erase(secondIndex);

			mOccupationBoard.emplace(arrIndexToCoord[firstIndex], firstIndex);
			mOccupationBoard.emplace(arrIndexToCoord[secondIndex], secondIndex);
		}
		if (!beginning)
			vHistory.pop_back();
		else
			vHistory.clear();

		InitRemovable();

		bIsLockedFromMove = false;

		if (!beginning)
		{
			for (const auto& it : vHistory)
			{
				RemoveTile(it.first);
				RemoveTile(it.second);

				std::vector<int> vMove;
				vMove.emplace_back(it.first);
				vMove.emplace_back(it.second);

				bIsLockedFromMove = bIsLockedFromMove ? true : CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, vMove);
			}

			SetMoves();
		}


		return true;
	}
}

// For the graphic board.
bool Board::TakeBack(const uint8_t direction)
{
	if (TakeBack(false))
	{
		SortBoard(direction);

		return true;
	}
	else
	{
		return false;
	}
}

// For the graphic board.
bool Board::GoBeginning(const uint8_t direction)
{
	if (TakeBack(true))
	{
		SortBoard(direction);

		SetMoves();

		return true;
	}
	else
	{
		return false;
	}
}

bool Board::MoveForward(bool end)
{
	if (vForwardHistory.empty())
	{
		return false;
	}
	else
	{
		const auto & it = vForwardHistory.back();
		const auto firstIndex = it.first;
		const auto secondIndex = it.second;

		vForwardHistory.pop_back();

		RemoveTile(it.first);
		RemoveTile(it.second);

		vHistory.emplace_back(std::make_pair(it.first, it.second));

		IsLockedFromMove();

		if(!end)
			SetMoves();

		return true;
	}
}

bool Board::GoEnd()
{
	if (!vForwardHistory.empty())
	{
		while (!vForwardHistory.empty())
		{
			MoveForward(true);
		}

		SetMoves();

		return true;
	}
	else
	{
		return false;
	}
}

#ifdef _DEBUG
#ifndef BENCHMARK
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
		if (CheckIfLockedFromStart(vLogicalBoard, mIndexToTile))
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

void Board::InitBoardLockedHorizontal(int test)
{
	// 0, 1, 2, 5, 7, 12, 18, 19, 24, 25, 27, 31, 34, 36, 44, 49, 50, 55, 61, 62, 64, 66
	// [0, 2, 21, 32]
	static int seed = 0;
	std::mt19937 e1(seed++);
	std::cout << "******************************************* " << seed - 1 << " *******************************************" << std::endl;

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

	int index = 0;
	switch (test)
	{
	default:
		for (; index < 144; ++index)
		{
			int domino = 0;
			do
			{
				domino = uniform_dist(e1);
			} while (tempDominos[domino] == 0);
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 0:
		for (; index < 4; ++index)
		{
			int domino = 0;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}

		for (; index < 8; ++index)
		{
			int domino = 1;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 1:
		for (; index < 4; ++index)
		{
			int domino = 0;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}

		for (; index < 8; ++index)
		{
			int domino = 0;
			do
			{
				domino = uniform_dist(e1);
			} while (domino == 1 || tempDominos[domino] == 0);
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		for (; index < 12; ++index)
		{
			int domino = 1;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 2:
		for (; index < 4; ++index)
		{
			int domino = 0;
			do
			{
				domino = uniform_dist(e1);
			} while (domino == 1 || domino == 0 || tempDominos[domino] == 0);
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		for (; index < 8; ++index)
		{
			int domino = 0;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}

		for (; index < 12; ++index)
		{
			int domino = 1;
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 3:
		for (; index < 144; ++index)
		{
			int domino = 0;
			//if (index == 0x8C || index == 0x8D || index == 0x8E || index == 0x2B)
			if (index == 0x8C || index == 0x2B || index == 0x21 || index == 0x2D)
			{
				domino = 1;
			}
			else if (index == 0x1E || index == 0x1F || index == 0x23 || index == 0x24)
			{
				domino = 2;
			}
			else if (index == 0x20 || index == 0x22 || index == 0x26 || index == 0x28)
			{
				domino = 3;
			}
			else
			{
				do
				{
					domino = uniform_dist(e1);
				} while (domino == 1 || domino == 2 || domino == 3 || tempDominos[domino] == 0);
			}
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 4:
		for (; index < 144; ++index)
		{
			int domino = 0;
			//if (index == 0x8C || index == 0x8D || index == 0x8E || index == 0x2B)
			if (index == 0x8C || index == 0x2B || index == 0x21 || index == 0x2D)
			{
				domino = 1;
			}
			else if (index == 0x1E || index == 0x1F || index == 0x23 || index == 0x2A)
			{
				domino = 2;
			}
			else if (index == 0x20 || index == 0x22 || index == 0x2C || index == 0x2E)
			{
				domino = 3;
			}
			else
			{
				do
				{
					domino = uniform_dist(e1);
				} while (domino == 1 || domino == 2 || domino == 3 || tempDominos[domino] == 0);
			}
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	case 5:
		for (; index < 144; ++index)
		{
			int domino = 0;
			//if (index == 0x8C || index == 0x8D || index == 0x8E || index == 0x2B)
			if (index == 0x8C || index == 0x2B || index == 0x21 || index == 0x2D)
			{
				domino = 1;
			}
			else if (index == 0x1E || index == 0x1F || index == 0x23 || index == 0x2A)
			{
				domino = 2;
			}
			else if (index == 0x20 || index == 0x22 || index == 0x2C || index == 0x2E)
			{
				domino = 3;
			}
			else
			{
				do
				{
					domino = uniform_dist(e1);
				} while (domino == 1 || domino == 2 || domino == 3 || tempDominos[domino] == 0);
			}
			--tempDominos[domino];
			mOccupationBoard[arrIndexToCoord[index]] = index;
			const auto coord = arrIndexToBoardCoord[index];
			vLogicalBoard.emplace_back(TileAndIndex(domino, index, std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), std::get<4>(coord)));
			mIndexToTile.emplace(index, Tile(domino));
		}
		break;
	}

	/*
	for (; index < 144; ++index)
	{
		int domino = 0;
		do
		{
			domino = uniform_dist(e1);
		} while (tempDominos[domino] == 0);
		--tempDominos[domino];
		mOccupationBoard[arrIndexToCoord[index]] = index;
		vLogicalBoard.emplace_back(TileAndIndex(domino, index));
		mIndexToTile.emplace(index, Tile(domino));
	}
	/**/

	arrRemovable = InitRemovable();

	SetMoves();
}

bool Board::TestLocked()
{
	std::vector<std::pair<int, int>> Locked;
	std::map<int, int> causes;
	int cause;
	for (int i = 0; i < 10000; ++i)
	//int i = 2;
	{
		//InitBoardLockedHorizontal(i);
		InitBoard();
		if (CheckIfLockedFromStart(vLogicalBoard, mIndexToTile, &cause))
		{
			Locked.emplace_back(std::make_pair(i, cause));
			causes[cause] += 1;
		}
	}

	std::cout << std::endl;

	if (!Locked.empty())
	{
		auto it = Locked.begin();
		for (; it != Locked.end() - 1; ++it)
		{
			std::cout << std::dec << "(" << it->first << ";" << it->second << "), ";
		}
		std::cout << std::dec << "(" << it->first << ";" << it->second << ")." << std::endl;
	}

	std::cout << "Bloqués : " << std::dec << Locked.size() << "." << std::endl;

	for (const auto& cause : causes)
		std::cout << "Cause : (" << std::dec << cause.first << "; " << cause.second << ")." << std::endl;

	return false;
}
#endif
#endif
