#pragma once
#include "Board.h"

constexpr std::array<std::array<std::array<int, 4>, 8>, 12> InitBoardCoordToIndex(const std::array<std::array<std::array<bool, 4>, 8>, 12>& BasePattern)
{
	std::array<std::array<std::array<int, 4>, 8>, 12> BaseTurtlePattern;
	int index = 0;
	for (int z = 0; z < 4; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) {
					BaseTurtlePattern[x][y][z] = index++;
				}
				else
				{
					BaseTurtlePattern[x][y][z] = -1;
				}
			}
		}
	}

	return BaseTurtlePattern;
}

constexpr std::array<std::tuple<int, int, int>, 140> InitIndexToBoardCoord(const std::array<std::array<std::array<bool, 4>, 8>, 12>& BasePattern)
{
	std::array<std::tuple<int, int, int>, 140> BaseTurtlePatternToCoord;
	int index = 0;
	for (int z = 0; z < 4; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) {
					BaseTurtlePatternToCoord[index++] = { x, y, z };
				}
			}
		}
	}

	return BaseTurtlePatternToCoord;
}

constexpr std::array < std::array < std::pair<int, int>, 4>, 8> InitHorizontalLimits(std::array<std::array<std::array<int, 4>, 8>, 12> BaseTurtlePattern)
{
	std::array < std::array < std::pair<int, int>, 4>, 8> HorizontalLimits;

	for (int z = 0; z < 4; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			int x = 0;
			int first = -1;
			int second = -1;
			for (; x < 12; ++x)
			{
				if (BaseTurtlePattern[x][y][z] != -1) {
					first = x;
					break;
				}
			}
			for (; x < 12; ++x)
			{
				second = x;
				if (BaseTurtlePattern[x][y][z] == -1)
				{
					--second;
					break;
				}
			}

			HorizontalLimits[y][z] = std::make_pair(first, second);
		}
	}

	return HorizontalLimits;
}

// Gets index from position (Padlocks not in).
constexpr std::array<std::array<std::array<int, 4>, 8>, 12> BoardCoordToIndex = InitBoardCoordToIndex(BasePattern);
// Gets position from the index (Padlocks not in).
constexpr std::array<std::tuple<int, int, int>, 140> IndexToBoardCoord = InitIndexToBoardCoord(BasePattern);
// Limits on horizontal lines (Padlocks not in).
constexpr std::array < std::array < std::pair<int, int>, 4>, 8> HorizontalLimits = InitHorizontalLimits(BoardCoordToIndex);

inline uint8_t EvalMoveMaxBlock(
	const std::vector<int>& Move,
	std::map<int, Domino>& TilesMap);

inline void RemoveTile(
	const int index,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, Domino>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<DominoIndex>& LogicalBoardRemoved,
	std::vector<int>& RemovableWasTrue,
	std::vector<int>& RemovableWasFalse,
	std::map<int, Domino>& TilesMapRemoved,
	std::vector<int>& WhatsLeftRemoved,
	std::map<Coordinates, int>& mOccupationBoardRemoved
)
{
	TilesMapRemoved.emplace(index, TilesMap.find(index)->second);
	TilesMap.erase(index);

	std::vector<DominoIndex>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && it->index != index; ++it);
	auto coord = IndexToCoord[it->index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;

	LogicalBoardRemoved.emplace_back(*it);
	LogicalBoard.erase(it);
	mOccupationBoardRemoved[coord] = index;
	mOccupationBoard.erase(coord);

	auto itWL = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (itWL != WhatsLeft.end())
	{
		WhatsLeftRemoved.emplace_back(*itWL);
		WhatsLeft.erase(itWL);
	}

	if (Removable[index]) RemovableWasTrue.emplace_back(index);
	Removable[index] = false;

	if (index == 0x8F)
	{
		if (!Removable[0x88]) RemovableWasFalse.emplace_back(0x88);
		if (!Removable[0x89]) RemovableWasFalse.emplace_back(0x89);
		if (!Removable[0x8A]) RemovableWasFalse.emplace_back(0x8A);
		if (!Removable[0x8B]) RemovableWasFalse.emplace_back(0x8B);

		Removable[0x88] = true;
		Removable[0x89] = true;
		Removable[0x8A] = true;
		Removable[0x8B] = true;
	}
	else if (index == 0x8C)
	{
		if (!Removable[0x1E]) RemovableWasFalse.emplace_back(0x1E);
		if (!Removable[0x2A]) RemovableWasFalse.emplace_back(0x2A);
		Removable[0x1E] = true;
		Removable[0x2A] = true;
	}
	else if (index == 0x8E)
	{
		if (!Removable[0x8D]) RemovableWasFalse.emplace_back(0x8D);
		Removable[0x8D] = true;
	}
	else if (index == 0x8D)
	{
		if (!Removable[0x29]) RemovableWasFalse.emplace_back(0x29);
		if (!Removable[0x35]) RemovableWasFalse.emplace_back(0x35);
		Removable[0x29] = true;
		Removable[0x35] = true;
	}
	else
	{
		if (x < 11 && mOccupationBoard.contains(Coordinates(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x + 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x + 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x - 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
		}
	}
}

// returns { Nombre de dominos jouables sur ce coup }
inline int BuildEvalMoves(const std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::const_iterator& itFirst)
{
	int moveEval = 0;
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).domino;
		auto itNext = itFirst;
		bool bEmpty = true;
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
				if (bEmpty)
				{
					++moveEval;
					bEmpty = false;
				}
				++moveEval;
				++itFirst;
			}
		} while (itNext != RemovableBoard.end());

		moveEval += BuildEvalMoves(RemovableBoard, ++itFirst);
	}
	return moveEval;
}

// returns { Nombre de dominos jouables sur ce coup , nombre de dominos débloqués }
inline std::pair<int, int> EvalMoves(const std::vector<DominoIndex>& LogicalBoard, const std::array<bool, 144>& Removable)
{
	std::vector<DominoIndex> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	if (Removable.size() == LogicalBoard.size())
		return { 0xFFFF, 0xFFFF };

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const DominoIndex& left, const DominoIndex& right)
		{
			return left.domino < right.domino;
		});
	auto itFirst = RemovableBoard.begin();

	return { BuildEvalMoves(RemovableBoard, itFirst), RemovableBoard.size() };
}

inline void BuildMoves(const std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::const_iterator& itFirst, std::vector<std::vector<int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).domino;
		auto itNext = itFirst;
		std::vector<int> temp;
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
				if (temp.empty())
					temp.emplace_back(itFirst->index);
				temp.emplace_back(itNext->index);
				++itFirst;
			}
		} while (itNext != RemovableBoard.end());

		if (!temp.empty())
		{
			if (temp.size() == 3)
			{
				// Little issue with my trick...
				Moves.emplace_back(std::vector<int>{temp[0], temp[1]});
				Moves.emplace_back(std::vector<int>{temp[0], temp[2]});
				Moves.emplace_back(std::vector<int>{temp[1], temp[2]});
			}
			else
			{
				Moves.emplace_back(temp);
			}
		}

		BuildMoves(RemovableBoard, ++itFirst, Moves);
	}
}

inline void SetMoves(const std::vector<DominoIndex>& LogicalBoard, const std::array<bool, 144>& Removable, std::vector<std::vector<int>>& Moves)
{
	std::vector<DominoIndex> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const DominoIndex& left, const DominoIndex& right)
		{
			return left.domino < right.domino;
		});
	auto itFirst = RemovableBoard.begin();
	Moves.clear();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

static std::map<uint64_t, std::array<uint64_t, 144 / 8>> hashtable;

#define FNV_offset_basis 0xcbf29ce484222325ULL
#define FNV_prime 0x100000001b3ULL

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
inline uint64_t getFNV1(const std::map<int, Domino>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second.rang);
	}

	uint64_t hash = FNV_offset_basis;

	for (int i = 0; i < 144; ++i)
	{
		hash *= FNV_prime;
		hash ^= tileTab[i];
	}

	return hash;
}

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
inline uint64_t getFNV1a(const std::map<int, Domino>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second.rang);
	}

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}

	return hash;
}

inline bool stopNow(const std::map<int, Domino>& TilesMap
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second.rang);
	}

	std::array<uint64_t, 144 / 8> boardDescription;

	// All numbers are 8 bits.
	// Stored 8 by 8 in 18 slides of 64 bits.
	for (int i = 0; i < 144; i += 8)
	{
		uint64_t temp = tileTab[i] << 56;
		temp |= tileTab[i + 1] << 48;
		temp |= tileTab[i + 2] << 40;
		temp |= tileTab[i + 3] << 32;
		temp |= tileTab[i + 4] << 24;
		temp |= tileTab[i + 5] << 16;
		temp |= tileTab[i + 6] << 8;
		temp |= tileTab[i + 7];
		boardDescription[i >> 3] = temp;
	}

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}

	if (hashtable.contains(hash))
	{
		auto boardDescriptionInHashtable = hashtable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
#ifdef _DEBUG
				std::cout << "Collision de hash, " << positions << std::endl;
#endif
				hashtable[hash] = boardDescription;
				return false;
			}
		}
		return true;
	}
	else
	{
		auto temp = std::make_pair(hash, boardDescription);
		hashtable.emplace(temp);
		return false;
	}
}

// returns { Nombre de dominos jouables sur ce coup , nombre de dominos débloqués }
inline std::pair<int, int> BruteForceOrderingEval(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, Domino>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<DominoIndex> LogicalBoardRemoved;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, Domino> TilesMapRemoved;
	std::vector<int> WhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& index : Move)
	{
		RemoveTile(index,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardRemoved, RemovableWasTrue, RemovableWasFalse, TilesMapRemoved, WhatsLeftRemoved, mOccupationBoardRemoved);
	}

	auto moveEval = EvalMoves(LogicalBoard, Removable);

	for (auto& item : LogicalBoardRemoved) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapRemoved) TilesMap.emplace(item.first, item.second);
	for (auto& item : WhatsLeftRemoved) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return moveEval;
}

inline bool SolveRec(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, Domino>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	std::vector<std::vector<int>> newMoves;

	std::vector<DominoIndex> LogicalBoardRemoved;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, Domino> TilesMapRemoved;
	std::vector<int> WhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& move : Move)
	{
		RemoveTile(move,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardRemoved, RemovableWasTrue, RemovableWasFalse, TilesMapRemoved, WhatsLeftRemoved, mOccupationBoardRemoved);
	}
	Solution.emplace_back(std::make_pair(Move[0], Move[1]));
	if (Move.size() == 4)
	{
		full = true;
		Solution.emplace_back(std::make_pair(Move[2], Move[3]));
	}

	SetMoves(LogicalBoard, Removable, newMoves);

	auto ret = LogicalBoard.empty();
	if (!ret)
	{
		if (!newMoves.empty())
		{
			if (!stopNow(TilesMap
#ifdef _DEBUG
				, positions
#endif
			))
			{
				std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> sortedMoves;
				for (auto& move : newMoves)
				{
					auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard);
					auto jouables = evalBruteForceOrderingEval.first;
					auto debloques = evalBruteForceOrderingEval.second;
					auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, TilesMap);
					sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
					//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock)));
					//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques)));
				}

				std::sort(sortedMoves.begin(), sortedMoves.end(),
					[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
					{
						return
							left.first.size() > right.first.size() ||
							left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
					});

				for (auto& move : sortedMoves)
				{
					ret = SolveRec(move.first, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution
#ifdef _DEBUG
						, ++positions
#endif
					);

					if (ret) break;
				}
			}
		}
		if (!ret)
		{
			if (full)
				Solution.pop_back();
			Solution.pop_back();
		}
	}
	/**/
	for (auto& item : LogicalBoardRemoved) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapRemoved) TilesMap.emplace(item.first, item.second);
	for (auto& item : WhatsLeftRemoved) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;
	/**/

	return ret;
}

/*
A
A
A
A
*/
inline bool isCenterLocked(int index, std::map<int, Domino>& TilesMap)
{
	std::vector<Domino> dominos;
	int dec1 = ((index - 0x88) >> 1) << 1;
	int dec2 = dec1 << 1;
	dominos.emplace_back(TilesMap.find(index)->second);
	dominos.emplace_back(TilesMap.find(index - 0x0B - dec1)->second);
	dominos.emplace_back(TilesMap.find(index - 0x1B - dec2)->second);
	dominos.emplace_back(TilesMap.find(index - 0x3F - dec2)->second);

	return (dominos[0].appairage == dominos[1].appairage && dominos[0].appairage == dominos[2].appairage && dominos[0].appairage == dominos[3].appairage);
}


inline bool checkIfLocked(int x, int y, int z)
{
	if (z == 3)
	{
		// z = {z .. z - 3} <=> z = {0 .. 3}
		// To check A/X/X/X & B/X/X/X with 3 A and 3 B among X when there are 4 A and 4 B left.
		// Among all z = 3
		// A/A when there are only 2 A left.
		// A/.../A/... when there are only 2 A left : generalisation.
		// A/.../B/.../ & B/.../A/... when there are only 2 A and 2 B left : generalisation.
	}
	if (z > 1)
	{
		// z = { z .. z - 2 }
		// No idea yet about what I need to look for on 3 tiles high pile.
	}
	if (z > 0)
	{
		// z = { z .. z - 1 }
		// To check A/B B/A when 2 A and 2 B are left
	}

	{
		// Whatever needs to be check horizontaly 
		// AAAA****BBBB
		// AA****BBBB and BBBB****AA when only 2 A are left
		// AA****BB when there are only 2 A and 2 B left
	}

	return false;
}
/*
	A  AB
	A  BA
	A  AB
	A  BA

	Out : AA/BB
	(n+1) AB
	(n+0) BA

	0x02 -> 0x51 first square
	0x00 -> 0x53 first floor
	0x54 -> 0x77 second floor
	0x77 -> 0x87 third floor
	0x88 -> 0x8b fourth floor
*/
inline bool CheckIfLockedFromStart(const std::map<int, Domino>& TilesMap)
{
	// Index -> domino
	// std::map<int, Domino>& TilesMap
	
	if (TilesMap.size() < 144)
		return false;

	if (TilesMap.contains(0x8F)) // Useless condition for a starting pos. I keep it for a probable future copy and paste.
	{
		auto bestPadlock = TilesMap.find(0x8F)->second.appairage;

		std::vector<int> startPos;
		startPos.emplace_back(0x88);
		startPos.emplace_back(0x89);
		startPos.emplace_back(0x8A);
		startPos.emplace_back(0x8B);

		int pairs = 0;
		for (const auto& index : startPos)
		{
			auto c = IndexToBoardCoord[index];
			auto x = std::get<0>(c);
			auto y = std::get<1>(c);
			auto z = std::get<2>(c);
			if (bestPadlock == TilesMap.find(BoardCoordToIndex[x][y][z])->second.appairage) ++pairs;
			if (pairs == 3)
				return true;
			if (bestPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 1])->second.appairage) ++pairs;
			if (pairs == 3)
				return true;
			if (bestPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 2])->second.appairage) ++pairs;
			if (pairs == 3)
				return true;
			if (bestPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 3])->second.appairage) ++pairs;
			if (pairs == 3)
				return true;
		}
	}

	std::vector<int> startPos;
	// Useless conditions for a starting pos. I keep them for a probable future copy and paste.
	if (TilesMap.contains(0x88)) startPos.emplace_back(0x88);
	if (TilesMap.contains(0x89)) startPos.emplace_back(0x89);
	if (TilesMap.contains(0x8A)) startPos.emplace_back(0x8A);
	if (TilesMap.contains(0x8B)) startPos.emplace_back(0x8B);

	if (!startPos.empty()) // Useless condition for a starting pos. I keep it for a probable future copy and paste.
	{
		// Pure vertical lock
		for (const auto& index : startPos)
		{
			auto c = IndexToBoardCoord[index];
			auto x = std::get<0>(c);
			auto y = std::get<1>(c);
			auto z = std::get<2>(c);
			// All of this can be harcoded. I keep it like this for a probable future copy and paste.
			auto firstPadlock = TilesMap.find(BoardCoordToIndex[x][y][z])->second.appairage;
			if (
				firstPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 1])->second.appairage &&
				firstPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 2])->second.appairage &&
				firstPadlock == TilesMap.find(BoardCoordToIndex[x][y][z - 3])->second.appairage
				)
				return true;
		}
	}

	// Doesn't work if not a starting pos. There could be holes in the lines.
	for(int z = 3; z >= 0; --z)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto horizontalLimits = HorizontalLimits[y][z];
			if (horizontalLimits.second - horizontalLimits.first + 1 > 7)
			{
				auto first = TilesMap.find(BoardCoordToIndex[horizontalLimits.first][y][z])->second;
				auto last = TilesMap.find(BoardCoordToIndex[horizontalLimits.second][y][z])->second;
				auto firstCount = 0;
				auto lastCount = 0;
				for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
				{
					if (TilesMap.find(BoardCoordToIndex[x][y][z])->second.appairage == first.appairage) ++firstCount;
					if (TilesMap.find(BoardCoordToIndex[x][y][z])->second.appairage == last.appairage) ++lastCount;
				}
				if (firstCount == lastCount && lastCount == 4)
					return true;
			}
		}
	}
	return false;
}

// New move container to remove the tiles 2 at once or 4 at once.
inline void ConvertMovesToVector(const std::vector<std::pair<int, int>>& oldMoves, std::vector<std::vector<int>>& Moves)
{
	for (int index = 0; index < oldMoves.size();)
	{
		std::vector<int> temp;
		temp.emplace_back(oldMoves[index].first);
		temp.emplace_back(oldMoves[index].second);
		if (oldMoves.size() >= (index + 6) && oldMoves[index].first == oldMoves[index + 1].first && oldMoves[index].first == oldMoves[index + 2].first)
		{
			temp.emplace_back(oldMoves[index + 5].first);
			temp.emplace_back(oldMoves[index + 5].second);
			index += 6;
		}
		else
		{
			++index;
		}
		Moves.emplace_back(temp);
	}
}

inline uint8_t EvalMoveMaxBlock(
	const std::vector<int>& Move,
	std::map<int, Domino>& TilesMap)
{
	uint8_t blockValue = 0;
	for (const auto& index : Move)
	{
		// Horizontal block value
		if (index == 0x8C)
		{
			// Horizontal block value
			uint8_t tempBlockValue = 0; 

			// Droite haut
			for (int x = 0; x < 12 && (TilesMap.contains(BoardCoordToIndex[x][3][0])); ++x, ++tempBlockValue);
			if (tempBlockValue > 0)
			{
				if (tempBlockValue == 12 && TilesMap.contains(0x8D))
					blockValue += TilesMap.contains(0x8E) ? tempBlockValue + 1 : tempBlockValue;
				else
					blockValue += tempBlockValue - 1;
			}

			tempBlockValue = 0;

			// Droite bas
			for (int x = 0; x < 12 && (TilesMap.contains(BoardCoordToIndex[x][4][0])); ++x, ++tempBlockValue);
			if (tempBlockValue > 0)
			{
				if (tempBlockValue == 12 && TilesMap.contains(0x8D))
					blockValue += TilesMap.contains(0x8E) ? tempBlockValue + 1 : tempBlockValue;
				else
					blockValue += tempBlockValue - 1;
			}
			// Vertical block value = z
			//blockValue += 0;
		}
		else if (index == 0x8D)
		{
			// Horizontal block value
			uint8_t tempBlockValue = 0;

			// Gauche haut
			for (int x = 11; x >=0 && (TilesMap.contains(BoardCoordToIndex[x][3][0])); --x, ++tempBlockValue);
			if (tempBlockValue > 0)
				blockValue += tempBlockValue == 12 && TilesMap.contains(0x8C) ? tempBlockValue : tempBlockValue - 1;

			tempBlockValue = 0;

			// Gauche bas
			for (int x = 11; x >= 0 && (TilesMap.contains(BoardCoordToIndex[x][4][0])); --x, ++tempBlockValue);
			if (tempBlockValue > 0)
				blockValue += tempBlockValue == 12 && TilesMap.contains(0x8C) ? tempBlockValue : tempBlockValue - 1;
			// Vertical block value = z
			//blockValue += 0;
		}
		else if (index == 0x8E)
		{
			if (TilesMap.contains(0x8D))
			{
				// Horizontal block value
				uint8_t tempBlockValue = 0;

				// Gauche haut
				for (int x = 11; x >= 0 && (TilesMap.contains(BoardCoordToIndex[x][3][0])); --x, ++tempBlockValue);
				if (tempBlockValue > 0)
					blockValue += tempBlockValue == 12 && TilesMap.contains(0x8C) ? tempBlockValue + 1 : tempBlockValue;

				tempBlockValue = 0;

				// Gauche bas
				for (int x = 11; x >= 0 && (TilesMap.contains(BoardCoordToIndex[x][4][0])); --x, ++tempBlockValue);
				if (tempBlockValue > 0)
					blockValue += tempBlockValue == 12 && TilesMap.contains(0x8C) ? tempBlockValue + 1 : tempBlockValue;
			}
			// Vertical block value = z
			//blockValue += 0;
		}
		else if (index == 0x8F)
		{
			// Horizontal block value
			//blockValue += 0;
			// Vertical block value = z
			// But here it blocks 4 tiles at once.
			blockValue += 4 * 4;
		}
		else
		{
			auto temp = IndexToBoardCoord[index];
			int x = std::get<0>(temp);
			int y = std::get<1>(temp);
			int z = std::get<2>(temp);
			int curX = 0;
			// Vertical block value = z
			blockValue += z;
			// Horizontal block value
			uint8_t tempBlockValue = 0;
			// Droite
			for (curX = x + 1; curX < 12 && (TilesMap.contains(BoardCoordToIndex[curX][y][z])); ++curX, ++tempBlockValue);
			if (tempBlockValue > 0)
			{
				if (z == 0 && (y == 3 || y == 4))
				{
					if ((x == 11 || curX == 12) && TilesMap.contains(0x8D))
						blockValue += TilesMap.contains(0x8E) ? tempBlockValue + 1 : tempBlockValue;
					else
						blockValue += tempBlockValue - 1;
				}
				else
				{
					blockValue += tempBlockValue - 1;
				}

			}
			tempBlockValue = 0;
			// Gauche
			for (curX = x - 1; curX >= 0 && (TilesMap.contains(BoardCoordToIndex[curX][y][z])); --curX, ++tempBlockValue);
			if (tempBlockValue > 0)
			{
				if (z == 0 && (y == 3 || y == 4))
				{
					if ((x == 0 || curX == -1) && TilesMap.contains(0x8C))
						blockValue += TilesMap.contains(0x8E) ? tempBlockValue + 1 : tempBlockValue;
					else
						blockValue += tempBlockValue - 1;
				}
				else
				{
					blockValue += tempBlockValue - 1;
				}
			}
		}
	}

	return blockValue;
}

// Max block to the rescue.
// Used when there is no choice meeting the requirements for the heuristic running.
// Section 3.3 from :
// https://iivq.net/scriptie/scriptie-bsc.pdf
inline bool tryMaxBlock(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, int>> sortedMoves;
		auto TilesMap = plateau.getTilesMap();
		for (auto& move : Moves)
		{
			auto eval = EvalMoveMaxBlock(move, TilesMap);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
			{
				return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline bool tryAlwaysFirst(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	while (!plateau.GetMovesLeft().empty())
	{
		auto itNextMove = plateau.GetMovesLeft().begin();
		Solution.emplace_back(*itNextMove);
		plateau.RemovePairOfTiles(itNextMove->first, itNextMove->second);
	}
	return plateau.IsEmpty();
}

// Par nombre de dominos enlevés en une seule fois
// Par nombre de coups jouables
// Par max bloqueurs
inline bool tryRandomHeuristics(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			auto jouables = evalBruteForceOrderingEval.first;
			auto debloques = evalBruteForceOrderingEval.second;
			auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, TilesMap);
			sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock))); // 25
			//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock))); // 22
			//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques))); // 23
			//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, evalEvalMoveMaxBlock, jouables))); // 13
			//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(evalEvalMoveMaxBlock, debloques, jouables))); // 17
			//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(evalEvalMoveMaxBlock, jouables, debloques))); // 19
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
			{
				return
					left.first.size() > right.first.size() ||
					left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
					(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
					(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline uint8_t EvalMoveGreedy(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, Domino>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<DominoIndex> LogicalBoardRemoved;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, Domino> TilesMapRemoved;
	std::vector<int> WhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;

	for (const auto& index : Move)
	{
		RemoveTile(index,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardRemoved, RemovableWasTrue, RemovableWasFalse, TilesMapRemoved, WhatsLeftRemoved, mOccupationBoardRemoved);
	}

	int removables = 0;
	for (const auto removable : Removable)
		++removables;

	for (auto& item : LogicalBoardRemoved) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapRemoved) TilesMap.emplace(item.first, item.second);
	for (auto& item : WhatsLeftRemoved) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return removables;
}

inline bool tryGreedy(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, int>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto eval = EvalMoveGreedy(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
			{
				return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

// Removes 4 then 2
// Supposed to remove 2 out of three free tiles before 2 out of 2.
// Hence the "Pseudo".
inline bool tryPseudoMultipleFirst(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		std::sort(Moves.begin(), Moves.end(), [](const std::vector<int>& left, const std::vector<int>& right) {return left.size() > right.size(); });
		auto Move = *Moves.begin();
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingPlayable(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
				return left.first.size() > right.first.size() || (left.first.size() == right.first.size() && left.second.first > right.second.first) ||
					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second));
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingPlayablePadlocksFirst(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
				return
					left.first.size() > right.first.size() || // Les 4 en premier.
					
					(left.first.size() == right.first.size() && left.second.first > right.second.first) || // Les jouables ensuite.

					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second)) || // Les déblocables

					// Les bloqueurs.
					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8F))) ||

					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8E))) ||

					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8E)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8D) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8D))) ||

					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8E)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8D) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8D)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8C) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8C)))
					;
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingFreed(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
					return left.first.size() > right.first.size() || (left.first.size() == right.first.size() && left.second.second > right.second.second) ||
						((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first > right.second.first));
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingFreedPadlocksFirst(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(plateau.GetMovesLeft(), Moves);

	while (!Moves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> sortedMoves;
		auto LogicalBoard = plateau.getLogicalBoard();
		auto Removable = plateau.getRemovable();
		auto TilesMap = plateau.getTilesMap();
		auto WhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (auto& move : Moves)
		{
			auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
			sortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(sortedMoves.begin(), sortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
				return
					(left.first.size() > right.first.size()) ||

					(left.first.size() == right.first.size() && left.second.second > right.second.second) ||

					((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first > right.second.first)) ||

					((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first == right.second.first) &&
					(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8F))) ||

					((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first == right.second.first) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8E))) ||

					((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first == right.second.first) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8E)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8D) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8D))) ||

					((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first == right.second.first) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8F) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8F)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8E) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8E)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8D) && right.first.end() != std::find(right.first.begin(), right.first.end(), 0x8D)) &&
						(left.first.end() != std::find(left.first.begin(), left.first.end(), 0x8C) && right.first.end() == std::find(right.first.begin(), right.first.end(), 0x8C)))
					;
			});

		auto Move = sortedMoves.begin()->first;
		if (Move.size() == 2)
		{
			plateau.RemovePairOfTiles(Move[0], Move[1]);
			Solution.emplace_back(std::make_pair(Move[0], Move[1]));
		}
		if (Move.size() == 4)
		{
			plateau.RemovePairOfTiles(Move[2], Move[3]);
			Solution.emplace_back(std::make_pair(Move[2], Move[3]));
		}

		Moves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), Moves);
	}
	return plateau.IsEmpty();
}

// Just to work on a copy.
inline bool SolveRecInit(const Board& plateau,
	std::vector<std::pair<int, int>> oldMoves,
	std::vector<DominoIndex> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, Domino> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{
#ifdef _DEBUG
	std::vector<DominoIndex> LogicalBoardRefForDebug = LogicalBoard;
	std::array<bool, 144> RemovableRefForDebug = Removable;
	std::map<int, Domino> TilesMapRefForDebug = TilesMap;
	std::vector<int> WhatsLeftRefForDebug = WhatsLeft;
	std::map<Coordinates, int> mOccupationBoardRefForDebug = mOccupationBoard;
#endif


	std::vector<std::pair<bool (*)(Board plateau, std::vector<std::pair<int, int>>& Solution), std::string>> vTries;

	vTries.push_back({tryAlwaysFirst, "tryAlwaysFirst"});
	vTries.push_back({ tryRandomHeuristics, "tryRandomHeuristics" });
	vTries.push_back({ tryGreedy, "tryGreedy" });
	vTries.push_back({ tryPseudoMultipleFirst, "tryPseudoMultipleFirst" });
	vTries.push_back({ tryBruteForceOrderingPlayable, "tryBruteForceOrderingPlayable" });
	vTries.push_back({ tryBruteForceOrderingPlayablePadlocksFirst, "tryBruteForceOrderingPlayablePadlocksFirst" });
	vTries.push_back({ tryBruteForceOrderingFreed, "tryBruteForceOrderingFreed" });
	vTries.push_back({ tryBruteForceOrderingFreedPadlocksFirst, "tryBruteForceOrderingFreedPadlocksFirst" });
	vTries.push_back({ tryMaxBlock, "tryMaxBlock" });

	Solution.clear();
	std::vector<std::pair<int, int>> SolutionTemp;

	for (const auto& func : vTries)
	{
		if (func.first(plateau, SolutionTemp))
		{
			Solution.clear();
			Solution = SolutionTemp;
#ifdef _DEBUG
			std::cout << "Solution : " << func.second << std::endl;
#else
			return true;
#endif
		}
		if (Solution.size() < SolutionTemp.size())
		{
			Solution.clear();
			Solution = SolutionTemp;
		}
		SolutionTemp.clear();
	}
#ifndef _DEBUG
	return true;
#endif
#ifdef _DEBUG
	for (auto& move : Solution)
		std::cout << move.first << ";" << move.second << std::endl;
#endif
	if (CheckIfLockedFromStart(TilesMap))
	{
#ifdef _DEBUG
		std::cout << "************" << std::endl;
		std::cout << "* Locked. *" << std::endl;
		std::cout << "************" << std::endl;
#endif
		return false;
	}

	return true;
	Solution.clear();
	bool ret = false;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(oldMoves, Moves);

	std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> sortedMoves;
	for (auto& move : Moves)
	{
		auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard);
		auto jouables = evalBruteForceOrderingEval.first;
		auto debloques = evalBruteForceOrderingEval.second;
		auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, TilesMap);
		sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
		//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock)));
		//sortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques)));
	}

	std::sort(sortedMoves.begin(), sortedMoves.end(),
		[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
		{
			return
				left.first.size() > right.first.size() ||
				left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
				(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
				(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
		});

#ifdef _DEBUG
	uint64_t positions = 0ULL;
#endif

	for (auto& move : sortedMoves)
	{
		ret = SolveRec(move.first, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution
#ifdef _DEBUG
			, positions
#endif
		);
		if (ret) break;
	}

#ifdef _DEBUG
	bool LogicalBoardOk = LogicalBoardRefForDebug.size() == LogicalBoard.size();
	bool RemovableOk = RemovableRefForDebug.size() == Removable.size();
	bool TilesMapOk = TilesMapRefForDebug.size() == TilesMap.size();
	bool WhatsLeftOk = WhatsLeftRefForDebug.size() == WhatsLeft.size();
	bool mOccupationBoardOk = mOccupationBoardRefForDebug.size() == mOccupationBoard.size();
	for (auto& item : LogicalBoardRefForDebug) LogicalBoardOk &= LogicalBoard.end() != std::find(LogicalBoard.begin(), LogicalBoard.end(), item);
	for (int i = 0; i < RemovableRefForDebug.size(); ++i) RemovableOk &= Removable[i] == RemovableRefForDebug[i];
	for (auto& item : TilesMapRefForDebug) TilesMapOk &= TilesMap.contains(item.first) && TilesMap.find(item.first)->second == item.second;
	for (auto& item : WhatsLeftRefForDebug) WhatsLeftOk &= WhatsLeftRefForDebug.end() != std::find(WhatsLeftRefForDebug.begin(), WhatsLeftRefForDebug.end(), item);
	for (auto& item : mOccupationBoardRefForDebug)  mOccupationBoardOk &= mOccupationBoard.contains(item.first) && mOccupationBoard[item.first] == item.second;

	std::cout << LogicalBoardOk << std::endl;
	std::cout << RemovableOk << std::endl;
	std::cout << TilesMapOk << std::endl;
	std::cout << WhatsLeftOk << std::endl;
	std::cout << mOccupationBoardOk << std::endl;
#endif

	// Transposition table must be cleared. The hash just means the positions has been seen at least once before. It doesn't mean it was in a lost game.
	// And the start position is always different.
	hashtable.clear();

	return ret;
}

#ifdef _DEBUG
int64_t testAll(const Board& plateau)
{
	std::vector<std::pair<bool (*)(Board plateau, std::vector<std::pair<int, int>>& Solution), std::string>> vTries;

	vTries.push_back({ tryAlwaysFirst, "tryAlwaysFirst" });
	vTries.push_back({ tryRandomHeuristics, "tryRandomHeuristics" });
	vTries.push_back({ tryGreedy, "tryGreedy" });
	vTries.push_back({ tryPseudoMultipleFirst, "tryPseudoMultipleFirst" });
	vTries.push_back({ tryBruteForceOrderingPlayable, "tryBruteForceOrderingPlayable" });
	vTries.push_back({ tryBruteForceOrderingPlayablePadlocksFirst, "tryBruteForceOrderingPlayablePadlocksFirst" });
	vTries.push_back({ tryBruteForceOrderingFreed, "tryBruteForceOrderingFreed" });
	vTries.push_back({ tryBruteForceOrderingFreedPadlocksFirst, "tryBruteForceOrderingFreedPadlocksFirst" });
	vTries.push_back({ tryMaxBlock, "tryMaxBlock" });


	std::vector<std::pair<int, int>> SolutionTemp;

	uint64_t ret = 0ULL;
	for (const auto& func : vTries)
	{
		ret = ret << 7;
		ret |= func.first(plateau, SolutionTemp) ? 1 : 0;
		SolutionTemp.clear();
	}

	return ret;
}
#endif
