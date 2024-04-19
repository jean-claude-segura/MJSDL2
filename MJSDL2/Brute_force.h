#pragma once
#include "Board.h"

inline void RemoveTile(
	const int index,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<DominoIndex>& LogicalBoardRemoved,
	std::vector<int>& RemovableWasTrue,
	std::vector<int>& RemovableWasFalse,
	std::map<int, int>& TilesMapRemoved,
	std::vector<int>& WhatsLeftRemoved,
	std::map<Coordinates, int>& mOccupationBoardRemoved
)
{
	TilesMapRemoved[index] = TilesMap[index];
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
					return
						(
							in.domino == domino ||
							(34 <= in.domino && in.domino < 38 && 34 <= domino && domino < 38) || // Saisons
							(38 <= in.domino && in.domino < 42 && 38 <= domino && domino < 42) // Fleurs.
							);
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

inline int EvalMoves(const std::vector<DominoIndex>& LogicalBoard, const std::array<bool, 144>& Removable)
{
	std::vector<DominoIndex> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	if (Removable.size() == LogicalBoard.size())
		return 0xFFFF;

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const DominoIndex& left, const DominoIndex& right)
		{
			return left.domino < right.domino;
		});
	auto itFirst = RemovableBoard.begin();

	return BuildEvalMoves(RemovableBoard, itFirst);
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
					return
						(
							in.domino == domino ||
							(34 <= in.domino && in.domino < 38 && 34 <= domino && domino < 38) || // Saisons
							(38 <= in.domino && in.domino < 42 && 38 <= domino && domino < 42) // Fleurs.
							);
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
inline uint64_t getFNV1(const std::map<int, int>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
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
inline uint64_t getFNV1a(const std::map<int, int>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
	}

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}

	return hash;
}

inline bool stopNow(const std::map<int, int>& TilesMap
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
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

inline uint8_t BruteForceOrderingEval(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<DominoIndex> LogicalBoardRemoved;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, int> TilesMapRemoved;
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

	auto moveEval = EvalMoves(LogicalBoard, Removable);

	for (auto& item : LogicalBoardRemoved) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapRemoved) TilesMap[item.first] = item.second;
	for (auto& item : WhatsLeftRemoved) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return moveEval;
}

inline bool SolveRec(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
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
	std::map<int, int> TilesMapRemoved;
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
				// Scooting step :
				std::vector<std::pair<std::vector<int>, int>> sortedMoves;
				for (auto& move : newMoves)
				{
					auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard);
					sortedMoves.emplace_back(std::make_pair(move, eval));
				}
				std::sort(sortedMoves.begin(), sortedMoves.end(),
					[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
					{
						return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
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
	for (auto& item : TilesMapRemoved) TilesMap[item.first] = item.second;
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
inline bool isCenterBlocked(int index, std::map<int, int>& TilesMap)
{
	int dominos[4];
	int dec1 = ((index - 0x88) >> 1) << 1;
	int dec2 = dec1 << 1;
	dominos[0] = TilesMap[index];
	dominos[1] = TilesMap[index - 0x0B - dec1];
	dominos[2] = TilesMap[index - 0x1B - dec2];
	dominos[3] = TilesMap[index - 0x3F - dec2];

	if (dominos[0] == dominos[1] && dominos[0] == dominos[2] && dominos[0] == dominos[3])
		return true;
	// Saisons :
	if (
		(34 <= dominos[0] && dominos[0] < 38) &&
		(34 <= dominos[1] && dominos[1] < 38) &&
		(34 <= dominos[2] && dominos[2] < 38) &&
		(34 <= dominos[3] && dominos[3] < 38)
		)
		return true;
	// Fleurs :
	if (
		(38 <= dominos[0] && dominos[0] < 42) &&
		(38 <= dominos[1] && dominos[1] < 42) &&
		(38 <= dominos[2] && dominos[2] < 42) &&
		(38 <= dominos[3] && dominos[3] < 42)
		)
		return true;
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
inline bool CheckIfBlocked(std::map<int, int>& TilesMap)
{
	// Index -> domino
	if (TilesMap.contains(0x88) && isCenterBlocked(0x88, TilesMap)) return true;
	if (TilesMap.contains(0x89) && isCenterBlocked(0x89, TilesMap)) return true;
	if (TilesMap.contains(0x8A) && isCenterBlocked(0x8A, TilesMap)) return true;
	if (TilesMap.contains(0x8B) && isCenterBlocked(0x8B, TilesMap)) return true;
	if (TilesMap.contains(0x8F));
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

inline bool tryRandomHeuristics(Board plateau, std::vector<std::pair<int, int>>& Solution)
{
	while (!plateau.GetMovesLeft().empty())
	{
		std::vector<std::pair<int, int>>::const_iterator itNextMove;
		int index = 0x8F;
		itNextMove = std::find_if(plateau.GetMovesLeft().begin(), plateau.GetMovesLeft().end(), [index](const std::pair<int, int>& move) { return move.first == index || move.second == index; });
		if (itNextMove == plateau.GetMovesLeft().end())
		{
			int index = 0x8C;
			itNextMove = std::find_if(plateau.GetMovesLeft().begin(), plateau.GetMovesLeft().end(), [index](const std::pair<int, int>& move) { return move.first == index || move.second == index; });
			if (itNextMove == plateau.GetMovesLeft().end())
			{
				int index = 0x8E;
				itNextMove = std::find_if(plateau.GetMovesLeft().begin(), plateau.GetMovesLeft().end(), [index](const std::pair<int, int>& move) { return move.first == index || move.second == index; });
				if (itNextMove == plateau.GetMovesLeft().end())
				{
					int index = 0x8D;
					itNextMove = std::find_if(plateau.GetMovesLeft().begin(), plateau.GetMovesLeft().end(), [index](const std::pair<int, int>& move) { return move.first == index || move.second == index; });
				}
			}
		}
		if (itNextMove == plateau.GetMovesLeft().end())
		{
			itNextMove = plateau.GetMovesLeft().begin();
		}
		Solution.emplace_back(*itNextMove);
		plateau.RemovePairOfTiles(itNextMove->first, itNextMove->second);
	}
	return plateau.IsEmpty();
}

inline uint8_t EvalMoveGreedy(
	const std::vector<int>& Move,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<DominoIndex> LogicalBoardRemoved;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, int> TilesMapRemoved;
	std::vector<int> WhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;

	for (const auto& move : Move)
	{
		RemoveTile(move,
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
	for (auto& item : TilesMapRemoved) TilesMap[item.first] = item.second;
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

inline bool tryBruteForceOrdering(Board plateau, std::vector<std::pair<int, int>>& Solution)
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
			auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, OccupationBoard);
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

// Just to work on a copy.
inline bool SolveRecInit(const Board& plateau,
	std::vector<std::pair<int, int>> oldMoves,
	std::vector<DominoIndex> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{
#ifdef _DEBUG
	std::vector<DominoIndex> LogicalBoardRefForDebug = LogicalBoard;
	std::array<bool, 144> RemovableRefForDebug = Removable;
	std::map<int, int> TilesMapRefForDebug = TilesMap;
	std::vector<int> WhatsLeftRefForDebug = WhatsLeft;
	std::map<Coordinates, int> mOccupationBoardRefForDebug = mOccupationBoard;
#endif


	std::vector<std::pair<bool (*)(Board plateau, std::vector<std::pair<int, int>>& Solution), std::string>> vTries;

	vTries.push_back({ tryAlwaysFirst, "tryAlwaysFirst" });
	vTries.push_back({ tryRandomHeuristics, "tryRandomHeuristics" });
	vTries.push_back({ tryGreedy, "tryGreedy" });
	vTries.push_back({ tryPseudoMultipleFirst, "tryPseudoMultipleFirst" });
	vTries.push_back({ tryBruteForceOrdering, "tryBruteForceOrdering" });

	Solution.clear();
	std::vector<std::pair<int, int>> SolutionTemp;

#ifdef _DEBUG
	for (const auto& func : vTries)
	{
		if (func.first(plateau, SolutionTemp))
		{
			Solution.clear();
			Solution = SolutionTemp;
			std::cout << "Solution : " << func.second << std::endl;
		}
		SolutionTemp.clear();
	}
#else
	for (const auto& func : vTries)
	{
		if (func.first(plateau, SolutionTemp))
		{
			Solution.clear();
			Solution = SolutionTemp;
			return true;
		}
		if (Solution.size() < SolutionTemp.size())
		{
			Solution.clear();
			Solution = SolutionTemp;
		}
		SolutionTemp.clear();
	}
#endif
#ifndef _DEBUG
	return true;
#endif
	return true;
	if (CheckIfBlocked(TilesMap))
		return false;

	bool ret = false;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	ConvertMovesToVector(oldMoves, Moves);

	// Scooting step :
	std::vector<std::pair<std::vector<int>, int>> sortedMoves;
	for (auto& move : Moves)
	{
		auto eval = BruteForceOrderingEval(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard);
		sortedMoves.emplace_back(std::make_pair(move, eval));
	}
	std::sort(sortedMoves.begin(), sortedMoves.end(),
		[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
		{
			return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
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
	for (auto& item : TilesMapRefForDebug) TilesMapOk &= TilesMap.contains(item.first) && TilesMap[item.first] == item.second;
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
