#pragma once
#include "Board.h"
#include <thread>
#include <future>

std::atomic<bool> stopSolverNow;
inline uint8_t EvalMoveMaxBlock(
	const std::vector<int>& vMove,
	std::map<int, Tile>& mIndexToTile);

inline bool CheckIfLockedFromMove(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToRemovedTile, const std::vector<int>& vMove);

inline void RemoveTile(
	const int index,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::vector<int>& vWhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<TileAndIndex>& vLogicalBoardRemoved,
	std::vector<int>& vRemovableWasTrue,
	std::vector<int>& vRemovableWasFalse,
	std::map<int, Tile>& mIndexToRemovedTile,
	std::vector<int>& vWhatsLeftRemoved,
	std::map<Coordinates, int>& mOccupationBoardRemoved
)
{
	mIndexToRemovedTile.emplace(index, mIndexToTile.find(index)->second);
	mIndexToTile.erase(index);

	std::vector<TileAndIndex>::iterator it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end() && it->Index != index; ++it);
	auto coord = arrIndexToCoord[it->Index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;

	vLogicalBoardRemoved.emplace_back(*it);
	vLogicalBoard.erase(it);
	mOccupationBoardRemoved[coord] = index;
	mOccupationBoard.erase(coord);

	auto itWL = std::find(vWhatsLeft.begin(), vWhatsLeft.end(), index);
	if (itWL != vWhatsLeft.end())
	{
		vWhatsLeftRemoved.emplace_back(*itWL);
		vWhatsLeft.erase(itWL);
	}

	if (arrRemovable[index]) vRemovableWasTrue.emplace_back(index);
	arrRemovable[index] = false;

	if (index == 0x8F)
	{
		if (!arrRemovable[0x88]) vRemovableWasFalse.emplace_back(0x88);
		if (!arrRemovable[0x89]) vRemovableWasFalse.emplace_back(0x89);
		if (!arrRemovable[0x8A]) vRemovableWasFalse.emplace_back(0x8A);
		if (!arrRemovable[0x8B]) vRemovableWasFalse.emplace_back(0x8B);

		arrRemovable[0x88] = true;
		arrRemovable[0x89] = true;
		arrRemovable[0x8A] = true;
		arrRemovable[0x8B] = true;
	}
	else if (index == 0x8C)
	{
		if (!arrRemovable[0x1E]) vRemovableWasFalse.emplace_back(0x1E);
		if (!arrRemovable[0x2A]) vRemovableWasFalse.emplace_back(0x2A);
		arrRemovable[0x1E] = true;
		arrRemovable[0x2A] = true;
	}
	else if (index == 0x8E)
	{
		if (!arrRemovable[0x8D]) vRemovableWasFalse.emplace_back(0x8D);
		arrRemovable[0x8D] = true;
	}
	else if (index == 0x8D)
	{
		if (!arrRemovable[0x29]) vRemovableWasFalse.emplace_back(0x29);
		if (!arrRemovable[0x35]) vRemovableWasFalse.emplace_back(0x35);
		arrRemovable[0x29] = true;
		arrRemovable[0x35] = true;
	}
	else
	{
		if (x < 11 && mOccupationBoard.contains(Coordinates(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x + 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x + 1, y, z)];
			if (!arrRemovable[index]) vRemovableWasFalse.emplace_back(index);
			arrRemovable[index] = true;
		}
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x - 1, y, z)];
			if (!arrRemovable[index]) vRemovableWasFalse.emplace_back(index);
			arrRemovable[index] = true;
		}
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!arrRemovable[index]) vRemovableWasFalse.emplace_back(index);
				arrRemovable[index] = true;
			}
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!arrRemovable[index]) vRemovableWasFalse.emplace_back(index);
				arrRemovable[index] = true;
			}
		}
	}
}

// returns { Nombre de dominos jouables sur ce coup }
inline int BuildEvalMoves(const std::vector<TileAndIndex>& vRemovableBoard, std::vector<TileAndIndex>::const_iterator& itFirst)
{
	int moveEval = 0;
	if (itFirst != vRemovableBoard.end())
	{
		auto domino = (*itFirst).TileObject;
		auto itNext = itFirst;
		bool bEmpty = true;
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
				if (bEmpty)
				{
					++moveEval;
					bEmpty = false;
				}
				++moveEval;
				++itFirst;
			}
		} while (itNext != vRemovableBoard.end());

		moveEval += BuildEvalMoves(vRemovableBoard, ++itFirst);
	}
	return moveEval;
}

// returns { Nombre de dominos jouables sur ce coup , nombre de dominos débloqués }
inline std::pair<int, int> EvalMoves(const std::vector<TileAndIndex>& vLogicalBoard, const std::array<bool, 144>& arrRemovable)
{
	std::vector<TileAndIndex> RemovableBoard; // (TileObject, Index)
	for (const auto& pair : vLogicalBoard)
	{
		if (arrRemovable[pair.Index]) RemovableBoard.emplace_back(pair);
	}

	if (arrRemovable.size() == vLogicalBoard.size())
		return { 0xFFFF, 0xFFFF };

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const TileAndIndex& left, const TileAndIndex& right)
		{
			return left.TileObject < right.TileObject;
		});
	auto itFirst = RemovableBoard.begin();

	return { BuildEvalMoves(RemovableBoard, itFirst), RemovableBoard.size() };
}

inline void BuildMoves(const std::vector<TileAndIndex>& RemovableBoard, std::vector<TileAndIndex>::const_iterator& itFirst, std::vector<std::vector<int>>& vMoves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).TileObject;
		auto itNext = itFirst;
		std::vector<int> temp;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, RemovableBoard.end(),
				[domino](const TileAndIndex& in)
				{
					return in.TileObject.Pairing == domino.Pairing;
				}
			);
			if (itNext != RemovableBoard.end())
			{
				if (temp.empty())
					temp.emplace_back(itFirst->Index);
				temp.emplace_back(itNext->Index);
				++itFirst;
			}
		} while (itNext != RemovableBoard.end());

		if (!temp.empty())
		{
			if (temp.size() == 3)
			{
				// Little issue with my trick...
				vMoves.emplace_back(std::vector<int>{temp[0], temp[1]});
				vMoves.emplace_back(std::vector<int>{temp[0], temp[2]});
				vMoves.emplace_back(std::vector<int>{temp[1], temp[2]});
			}
			else
			{
				vMoves.emplace_back(temp);
			}
		}

		BuildMoves(RemovableBoard, ++itFirst, vMoves);
	}
}

inline void SetMoves(const std::vector<TileAndIndex>& vLogicalBoard, const std::array<bool, 144>& arrRemovable, std::vector<std::vector<int>>& vMoves)
{
	std::vector<TileAndIndex> RemovableBoard; // (TileObject, Index)
	for (const auto& pair : vLogicalBoard)
	{
		if (arrRemovable[pair.Index]) RemovableBoard.emplace_back(pair);
	}

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const TileAndIndex& left, const TileAndIndex& right)
		{
			return left.TileObject < right.TileObject;
		});
	auto itFirst = RemovableBoard.begin();
	vMoves.clear();
	BuildMoves(RemovableBoard, itFirst, vMoves);
}

static std::map<uint64_t, std::array<uint64_t, 144 / 8>> mTranspositionsTable;

#define FNV_offset_basis 0xcbf29ce484222325ULL
#define FNV_prime 0x100000001b3ULL

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
inline uint64_t getFNV1(const std::map<int, Tile>& mIndexToTile)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : mIndexToTile)
	{
		tileTab[item.first] = uint64_t(item.second.Rank);
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
inline uint64_t getFNV1a(const std::map<int, Tile>& mIndexToTile)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : mIndexToTile)
	{
		tileTab[item.first] = uint64_t(item.second.Rank);
	}

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}

	return hash;
}

std::mutex mtxStopNow;

inline bool stopNowParallel(const std::map<int, Tile>& mIndexToTile
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : mIndexToTile)
	{
		tileTab[item.first] = uint64_t(item.second.Rank);
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
	mtxStopNow.lock();
	if (mTranspositionsTable.contains(hash))
	{
		auto boardDescriptionInHashtable = mTranspositionsTable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
#ifdef _DEBUG
				std::cout << "Collision de hash, " << positions << std::endl;
#endif
				mTranspositionsTable[hash] = boardDescription;
				mtxStopNow.unlock();
				return false;
			}
		}
		mtxStopNow.unlock();
		return true;
	}
	else
	{
		auto temp = std::make_pair(hash, boardDescription);
		mTranspositionsTable.emplace(temp);
		mtxStopNow.unlock();
		return false;
	}
}

inline bool stopNow(const std::map<int, Tile>& mIndexToTile
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : mIndexToTile)
	{
		tileTab[item.first] = uint64_t(item.second.Rank);
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

	if (mTranspositionsTable.contains(hash))
	{
		auto boardDescriptionInHashtable = mTranspositionsTable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
#ifdef _DEBUG
				std::cout << "Collision de hash, " << positions << std::endl;
#endif
				mTranspositionsTable[hash] = boardDescription;
				return false;
			}
		}
		return true;
	}
	else
	{
		auto temp = std::make_pair(hash, boardDescription);
		mTranspositionsTable.emplace(temp);
		return false;
	}
}

// returns { Nombre de dominos jouables sur ce coup , nombre de dominos débloqués }
inline std::pair<int, int> BruteForceOrderingEval(
	const std::vector<int>& vMove,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::vector<int>& vWhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::vector<int> WhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& index : vMove)
	{
		RemoveTile(index,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			vWhatsLeft,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, WhatsLeftRemoved, mOccupationBoardRemoved);
	}

	auto moveEval = EvalMoves(vLogicalBoard, arrRemovable);

	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : WhatsLeftRemoved) vWhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return moveEval;
}

inline bool SolveRecParallel(
	const std::vector<int>& vMove,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::vector<int>& vWhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	std::vector<std::vector<int>> vNewMoves;

	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::vector<int> vWhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& move : vMove)
	{
		RemoveTile(move,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			vWhatsLeft,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, vWhatsLeftRemoved, mOccupationBoardRemoved);
	}
	vSolution.emplace_back(std::make_pair(vMove[0], vMove[1]));
	if (vMove.size() == 4)
	{
		full = true;
		vSolution.emplace_back(std::make_pair(vMove[2], vMove[3]));
	}

	SetMoves(vLogicalBoard, arrRemovable, vNewMoves);

	auto ret = vLogicalBoard.empty();
	if (!ret)
	{
		if (!CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, vMove) && !vNewMoves.empty())
		{
			if (!stopNowParallel(mIndexToTile
#ifdef _DEBUG
				, positions
#endif
			))
			{
				std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
				for (const auto& move : vNewMoves)
				{
					auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard);
					auto jouables = evalBruteForceOrderingEval.first;
					auto debloques = evalBruteForceOrderingEval.second;
					auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, mIndexToTile);
					vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
					//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock)));
					//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques)));
				}

				std::sort(vSortedMoves.begin(), vSortedMoves.end(),
					[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
					{
						return
							left.first.size() > right.first.size() ||
							left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
					});

				for (const auto& move : vSortedMoves)
				{
					ret = SolveRecParallel(move.first, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution
#ifdef _DEBUG
						, ++positions
#endif
					);

					if (ret)
						stopSolverNow = true;
					if (stopSolverNow)
						break;
				}
			}
		}
		if (!ret)
		{
			if (full)
				vSolution.pop_back();
			vSolution.pop_back();
		}
	}
	/**/
	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : vWhatsLeftRemoved) vWhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;
	/**/

	return ret;
}

inline bool SolveRec(
	const std::vector<int>& vMove,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::vector<int>& vWhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	std::vector<std::vector<int>> vNewMoves;

	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::vector<int> vWhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& move : vMove)
	{
		RemoveTile(move,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			vWhatsLeft,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, vWhatsLeftRemoved, mOccupationBoardRemoved);
	}
	vSolution.emplace_back(std::make_pair(vMove[0], vMove[1]));
	if (vMove.size() == 4)
	{
		full = true;
		vSolution.emplace_back(std::make_pair(vMove[2], vMove[3]));
	}

	SetMoves(vLogicalBoard, arrRemovable, vNewMoves);

	auto ret = vLogicalBoard.empty();
	if (!ret)
	{
		if (!CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, vMove) && !vNewMoves.empty())
		{
			if (!stopNow(mIndexToTile
#ifdef _DEBUG
				, positions
#endif
			))
			{
				std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
				for (const auto& move : vNewMoves)
				{
					auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard);
					auto jouables = evalBruteForceOrderingEval.first;
					auto debloques = evalBruteForceOrderingEval.second;
					auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, mIndexToTile);
					vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
					//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock)));
					//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques)));
				}

				std::sort(vSortedMoves.begin(), vSortedMoves.end(),
					[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
					{
						return
							left.first.size() > right.first.size() ||
							left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
							(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
					});

				for (const auto& move : vSortedMoves)
				{
					ret = SolveRec(move.first, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution
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
				vSolution.pop_back();
			vSolution.pop_back();
		}
	}
	/**/
	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : vWhatsLeftRemoved) vWhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;
	/**/

	return ret;
}

inline bool CheckIfLockedFromMove(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToRemovedTile, const std::vector<int>& vMove)
{
	if (vMove.size() == 4)
		return false;

	std::array<std::array<std::array<Tile, 4>, 8>, 12> arrBoard;

	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index < 140)
			arrBoard[tileAndIndex.X][tileAndIndex.Y][tileAndIndex.Z] = tileAndIndex.TileObject;
	}

	const auto pairing = mIndexToRemovedTile.find(vMove[0])->second.Pairing;

	std::vector<TileAndIndex> vPairedIndex;
	auto it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end(); ++it)
	{
		if (it->TileObject.Pairing == pairing)
		{
			vPairedIndex.emplace_back(*it);
			break;
		}
	}
	if (it != vLogicalBoard.end())
	{
		for (++it; it != vLogicalBoard.end(); ++it)
		{
			if (it->TileObject.Pairing == pairing)
			{
				vPairedIndex.emplace_back(*it);
				break;
			}
		}

		// Pure vertical lock.
		auto c1 = vPairedIndex[0];
		auto c2 = vPairedIndex[1];
		if (c1.X == c2.X && c1.Y == c2.Y && c1.DecX == c2.DecX && c1.DecY == c2.DecY)
			return true;
	}

	return false;
}

inline bool CheckIfLocked(const std::vector<TileAndIndex>& vLogicalBoard)
{
	std::array<std::array<std::array<Tile, 4>, 8>, 12> arrBoard = {};

	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index < 140)
			arrBoard[tileAndIndex.X][tileAndIndex.Y][tileAndIndex.Z] = tileAndIndex.TileObject;
	}

	std::array<int, 42> arrCountOnPairing;
	for (int i = 0; i < 42; ++i) arrCountOnPairing[i] = 0;
	for (const auto& tileAndIndex : vLogicalBoard)
	{
		++arrCountOnPairing[tileAndIndex.TileObject.Pairing];
	}

	for (int pairing = 0; pairing < 42; ++pairing)
	{
		if (arrCountOnPairing[pairing] != 2)
			continue;

		std::vector<TileAndIndex> vPairedIndex;
		auto it = vLogicalBoard.begin();
		for (; it != vLogicalBoard.end(); ++it)
		{
			if (it->TileObject.Pairing == pairing)
			{
				vPairedIndex.emplace_back(*it);
				break;
			}
		}
		if (it != vLogicalBoard.end())
		{
			for (++it; it != vLogicalBoard.end(); ++it)
			{
				if (it->TileObject.Pairing == pairing)
				{
					vPairedIndex.emplace_back(*it);
					break;
				}
			}
			// Pure vertical lock.
			auto c1 = vPairedIndex[0];
			auto c2 = vPairedIndex[1];
			if (c1.X == c2.X && c1.Y == c2.Y && c1.DecX == c2.DecX && c1.DecY == c2.DecY)
				return true;
		}
	}
	/*
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
	*/
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
inline bool CheckIfLockedFromStart(const std::map<int, Tile>& mIndexToTile, int* cause = NULL)
{
	// Index -> TileObject
	// std::map<int, Tile>& mIndexToTile
	
	if (mIndexToTile.size() < 144)
		return false;

	auto bestPadlock = mIndexToTile.find(0x8F)->second.Pairing;

	std::vector<int> vStartPos;
	vStartPos.emplace_back(0x88);
	vStartPos.emplace_back(0x89);
	vStartPos.emplace_back(0x8A);
	vStartPos.emplace_back(0x8B);

	/**/
	int pairs = 0;
	for (auto itIndex = vStartPos.begin(); itIndex != vStartPos.end(); ++itIndex)
	{
		auto c = arrIndexToBoardCoord[*itIndex];
		auto x = std::get<0>(c);
		auto y = std::get<1>(c);
		auto z = std::get<2>(c);

		auto firstPadlock = mIndexToTile.find(arrBoardCoordToIndex[x][y][z])->second.Pairing;

		// Blocked by the bestPadlock.
		if (bestPadlock == firstPadlock) ++pairs;
		if (pairs == 3)
			{ if (cause != NULL) { *cause = 1; }; return true; }
		if (bestPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 1])->second.Pairing) ++pairs;
		if (pairs == 3)
			{ if (cause != NULL) { *cause = 1; }; return true; }
		if (bestPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 2])->second.Pairing) ++pairs;
		if (pairs == 3)
			{ if (cause != NULL) { *cause = 1; }; return true; }
		if (bestPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 3])->second.Pairing) ++pairs;
		if (pairs == 3)
			{ if (cause != NULL) { *cause = 1; }; return true; }

		// Pure vertical lock.
		// All of this can be harcoded. I keep it like this for a probable future copy and paste.
		if (
			firstPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 1])->second.Pairing &&
			firstPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 2])->second.Pairing &&
			firstPadlock == mIndexToTile.find(arrBoardCoordToIndex[x][y][z - 3])->second.Pairing
			)
			{ if (cause != NULL) { *cause = 2; }; return true; }

		// Crossed vertical lock.
		if (itIndex < vStartPos.end() - 1)
		{
			auto itRef = itIndex;
			auto refAppairage = mIndexToTile.find(*itRef)->second.Pairing;
			auto refBoardCoord = arrIndexToBoardCoord[*itRef];
			int refX = std::get<0>(refBoardCoord);
			int refY = std::get<1>(refBoardCoord);
			int refZ = std::get<2>(refBoardCoord);
			for (auto itSecond = itRef + 1; itSecond != vStartPos.end(); ++itSecond)
			{
				auto secondAppairage = mIndexToTile.find(*itSecond)->second.Pairing;
				auto secondBoardCoord = arrIndexToBoardCoord[*itSecond];
				int secondX = std::get<0>(secondBoardCoord);
				int secondY = std::get<1>(secondBoardCoord);
				auto refCount = 0;
				auto secondCount = 0;
				for (int z = refZ; z >= 0; --z)
				{
					auto compAppairage = mIndexToTile.find(arrBoardCoordToIndex[refX][refY][z])->second.Pairing;
					if (refAppairage == compAppairage) ++refCount;
					if (secondAppairage == compAppairage) ++secondCount;
					compAppairage = mIndexToTile.find(arrBoardCoordToIndex[secondX][secondY][z])->second.Pairing;
					if (refAppairage == compAppairage) ++refCount;
					if (secondAppairage == compAppairage) ++secondCount;
					if (secondCount == 4 && refCount == secondCount)
						{ if (cause != NULL) { *cause = 3; }; return true; }
				}
			}
		}
	}

	// For left and right padlocks : funny blocking...
	// Doesn't work if not a starting pos. There could be padlocks missing.
	// First pass : check potential issues
	std::map<int, int> mPairingCount;
	std::map<int, int> mPairingFirst;
	std::map<int, int> mPairingLast;

	// Forced entries for the padlocks.
	auto leftPadlockPairing = mIndexToTile.find(0x8C)->second.Pairing;
	++mPairingCount[leftPadlockPairing];
	mPairingFirst[leftPadlockPairing] = -1;
	mPairingLast[leftPadlockPairing] = -1;

	auto rightPadlockPairing = mIndexToTile.find(0x8D)->second.Pairing;
	++mPairingCount[rightPadlockPairing];
	if (!mPairingFirst.contains(rightPadlockPairing))
		mPairingFirst[rightPadlockPairing] = 12 + 12;
	mPairingLast[rightPadlockPairing] = 12 + 12;

	auto rightRightPadlockPairing = mIndexToTile.find(0x8D)->second.Pairing;
	++mPairingCount[rightRightPadlockPairing];
	if (!mPairingFirst.contains(rightRightPadlockPairing))
		mPairingFirst[rightRightPadlockPairing] = 13 + 12;
	mPairingLast[rightRightPadlockPairing] = 13 + 12;

	// Creating a fake 24 tiles line to include the padlocks obstruction from the beginning to the end.
	for (int x = 0; x < 12; ++x)
	{
		// First get the tile.
		const auto& object = mIndexToTile.find(arrBoardCoordToIndex[x][3][0])->second;
		// Get the first occurence
		if (!mPairingFirst.contains(object.Pairing))
			mPairingFirst[object.Pairing] = x;
		// Get the last occurence
		mPairingLast[object.Pairing] = x;
		// Get the count of occurences
		++mPairingCount[object.Pairing];
	}
	for (int x = 0; x < 12; ++x)
	{
		// First get the tile.
		const auto& object = mIndexToTile.find(arrBoardCoordToIndex[x][4][0])->second;
		// Get the first occurence
		if (!mPairingFirst.contains(object.Pairing))
			mPairingFirst[object.Pairing] = x + 12;
		// Get the last occurence
		mPairingLast[object.Pairing] = x + 12;
		// Get the count of occurences
		++mPairingCount[object.Pairing];
	}
	// Remove the useless ones :
	for (const auto& item : mPairingCount)
	{
		if (item.second != 4)
		{
			mPairingFirst.erase(item.first);
			mPairingLast.erase(item.first);
		}
	}
	if (mPairingFirst.size() > 1) // Ok, now we have an issue...
	{
		auto itEnd = mPairingFirst.end();
		--itEnd;
		for (auto it = mPairingFirst.begin(); it != itEnd; ++it)
		{
			auto itNext = it;
			for (++itNext; itNext != mPairingFirst.end(); ++itNext)
			{
				// fAx < fBx && lAx < lBx
				if (it->second < itNext->second && mPairingLast.find(it->first)->second < mPairingLast.find(itNext->first)->second)
					if (cause != NULL) { *cause = 4; }; return true;
				// fBx < fAx && lBx < lAx
				/*if (it->second > itNext->second && mPairingLast.find(it->first)->second > mPairingLast.find(itNext->first)->second)
					if (cause != NULL) { *cause = 5; }; return true;*/
			}
		}
	}

	// Doesn't work if not a starting pos. There could be holes in the lines.
	for(int z = 3; z >= 0; --z)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto horizontalLimits = arrHorizontalLimits[y][z];
			// You need at least 8 slots to block 8 tiles...
			if (horizontalLimits.second - horizontalLimits.first + 1 > 7)
			{
				// First pass : check potential issues
				std::map<int, int> mPairingCount;
				std::map<int, int> mPairingFirst;
				std::map<int, int> mPairingLast;

				if (3 <= y && y <= 4 && z == 0)
				{
					// Fake entries for the padlocks.
					auto leftPadlockPairing = mIndexToTile.find(0x8C)->second.Pairing;
					++mPairingCount[leftPadlockPairing];
					mPairingFirst[leftPadlockPairing] = -1;
					mPairingLast[leftPadlockPairing] = -1;

					auto rightPadlockPairing = mIndexToTile.find(0x8D)->second.Pairing;
					++mPairingCount[rightPadlockPairing];
					if (!mPairingFirst.contains(rightPadlockPairing))
						mPairingFirst[rightPadlockPairing] = 12;
					mPairingLast[rightPadlockPairing] = 12;

					auto rightRightPadlockPairing = mIndexToTile.find(0x8D)->second.Pairing;
					++mPairingCount[rightRightPadlockPairing];
					if (!mPairingFirst.contains(rightRightPadlockPairing))
						mPairingFirst[rightRightPadlockPairing] = 13;
					mPairingLast[rightRightPadlockPairing] = 13;

					// Ponder
					int yPonder = y == 3 ? 4 : 3;
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
					{
						// First get the tile.
						const auto pairing = mIndexToTile.find(arrBoardCoordToIndex[x][yPonder][0])->second.Pairing;
						if (pairing == leftPadlockPairing || pairing == rightPadlockPairing || pairing == rightRightPadlockPairing)
							++mPairingCount[pairing];
					}
				}

				for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
				{
					// First get the tile.
					const auto& object = mIndexToTile.find(arrBoardCoordToIndex[x][y][z])->second;
					// Get the first occurence
					if (!mPairingFirst.contains(object.Pairing))
						mPairingFirst[object.Pairing] = x;
					// Get the last occurence
					if(!mPairingLast.contains(object.Pairing) || mPairingLast.find(object.Pairing)->second < x) // Because of the left and right blocker fake init before...
						mPairingLast[object.Pairing] = x;
					// Get the count of occurences
					++mPairingCount[object.Pairing];
				}
				// Remove the useless ones :
				for (const auto& item : mPairingCount)
				{
					if (item.second != 4)
					{
						mPairingFirst.erase(item.first);
						mPairingLast.erase(item.first);
					}
				}
				if (mPairingFirst.size() > 1) // Ok, now we have an issue...
				{
					auto itEnd = mPairingFirst.end();
					--itEnd;
					for (auto it = mPairingFirst.begin(); it != itEnd; ++it)
					{
						auto itNext = it;
						for (++itNext; itNext != mPairingFirst.end(); ++itNext)
						{
							// fAx < fBx && lAx < lBx
							if (it->second < itNext->second && mPairingLast.find(it->first)->second < mPairingLast.find(itNext->first)->second)
								if (cause != NULL) { *cause = 6; }; return true;
							// fBx < fAx && lBx < lAx
							/*if (it->second > itNext->second && mPairingLast.find(it->first)->second > mPairingLast.find(itNext->first)->second)
								if (cause != NULL) { *cause = 7; }; return true;*/
						}
					}
				}
			}
		}
	}
	{ if (cause != NULL) { *cause = 0; }; return false; }
}

// New move container to remove the tiles 2 at once or 4 at once.
inline void ConvertMovesToVector(const std::vector<std::pair<int, int>>& vOldMoves, std::vector<std::vector<int>>& vMoves)
{
	for (int index = 0; index < vOldMoves.size();)
	{
		std::vector<int> temp;
		temp.emplace_back(vOldMoves[index].first);
		temp.emplace_back(vOldMoves[index].second);
		if (vOldMoves.size() >= (index + 6) && vOldMoves[index].first == vOldMoves[index + 1].first && vOldMoves[index].first == vOldMoves[index + 2].first)
		{
			temp.emplace_back(vOldMoves[index + 5].first);
			temp.emplace_back(vOldMoves[index + 5].second);
			index += 6;
		}
		else
		{
			++index;
		}
		vMoves.emplace_back(temp);
	}
}

inline uint8_t EvalMoveMaxBlock(
	const std::vector<int>& vMove,
	std::map<int, Tile>& mIndexToTile)
{
	uint8_t lockingValue = 0;
	for (const auto& index : vMove)
	{
		// Horizontal locking value
		if (index == 0x8C)
		{
			// Horizontal locking value
			uint8_t tempLockingValue = 0; 

			// Droite haut
			for (int x = 0; x < 12 && (mIndexToTile.contains(arrBoardCoordToIndex[x][3][0])); ++x, ++tempLockingValue);
			if (tempLockingValue > 0)
			{
				if (tempLockingValue == 12 && mIndexToTile.contains(0x8D))
					lockingValue += mIndexToTile.contains(0x8E) ? tempLockingValue + 1 : tempLockingValue;
				else
					lockingValue += tempLockingValue - 1;
			}

			tempLockingValue = 0;

			// Droite bas
			for (int x = 0; x < 12 && (mIndexToTile.contains(arrBoardCoordToIndex[x][4][0])); ++x, ++tempLockingValue);
			if (tempLockingValue > 0)
			{
				if (tempLockingValue == 12 && mIndexToTile.contains(0x8D))
					lockingValue += mIndexToTile.contains(0x8E) ? tempLockingValue + 1 : tempLockingValue;
				else
					lockingValue += tempLockingValue - 1;
			}
			// Vertical locking value = z
			//lockingValue += 0;
		}
		else if (index == 0x8D)
		{
			// Horizontal locking value
			uint8_t tempLockingValue = 0;

			// Gauche haut
			for (int x = 11; x >=0 && (mIndexToTile.contains(arrBoardCoordToIndex[x][3][0])); --x, ++tempLockingValue);
			if (tempLockingValue > 0)
				lockingValue += tempLockingValue == 12 && mIndexToTile.contains(0x8C) ? tempLockingValue : tempLockingValue - 1;

			tempLockingValue = 0;

			// Gauche bas
			for (int x = 11; x >= 0 && (mIndexToTile.contains(arrBoardCoordToIndex[x][4][0])); --x, ++tempLockingValue);
			if (tempLockingValue > 0)
				lockingValue += tempLockingValue == 12 && mIndexToTile.contains(0x8C) ? tempLockingValue : tempLockingValue - 1;
			// Vertical locking value = z
			//lockingValue += 0;
		}
		else if (index == 0x8E)
		{
			if (mIndexToTile.contains(0x8D))
			{
				// Horizontal locking value
				uint8_t tempLockingValue = 0;

				// Gauche haut
				for (int x = 11; x >= 0 && (mIndexToTile.contains(arrBoardCoordToIndex[x][3][0])); --x, ++tempLockingValue);
				if (tempLockingValue > 0)
					lockingValue += tempLockingValue == 12 && mIndexToTile.contains(0x8C) ? tempLockingValue + 1 : tempLockingValue;

				tempLockingValue = 0;

				// Gauche bas
				for (int x = 11; x >= 0 && (mIndexToTile.contains(arrBoardCoordToIndex[x][4][0])); --x, ++tempLockingValue);
				if (tempLockingValue > 0)
					lockingValue += tempLockingValue == 12 && mIndexToTile.contains(0x8C) ? tempLockingValue + 1 : tempLockingValue;
			}
			// Vertical locking value = z
			//lockingValue += 0;
		}
		else if (index == 0x8F)
		{
			// Horizontal locking value
			//lockingValue += 0;
			// Vertical locking value = z
			// But here it blocks 4 tiles at once.
			lockingValue += 4 * 4;
		}
		else
		{
			auto temp = arrIndexToBoardCoord[index];
			int x = std::get<0>(temp);
			int y = std::get<1>(temp);
			int z = std::get<2>(temp);
			int curX = 0;
			// Vertical locking value = z
			lockingValue += z;
			// Horizontal locking value
			uint8_t tempLockingValue = 0;
			// Droite
			for (curX = x + 1; curX < 12 && (mIndexToTile.contains(arrBoardCoordToIndex[curX][y][z])); ++curX, ++tempLockingValue);
			if (tempLockingValue > 0)
			{
				if (z == 0 && (y == 3 || y == 4))
				{
					if ((x == 11 || curX == 12) && mIndexToTile.contains(0x8D))
						lockingValue += mIndexToTile.contains(0x8E) ? tempLockingValue + 1 : tempLockingValue;
					else
						lockingValue += tempLockingValue - 1;
				}
				else
				{
					lockingValue += tempLockingValue - 1;
				}

			}
			tempLockingValue = 0;
			// Gauche
			for (curX = x - 1; curX >= 0 && (mIndexToTile.contains(arrBoardCoordToIndex[curX][y][z])); --curX, ++tempLockingValue);
			if (tempLockingValue > 0)
			{
				if (z == 0 && (y == 3 || y == 4))
				{
					if ((x == 0 || curX == -1) && mIndexToTile.contains(0x8C))
						lockingValue += mIndexToTile.contains(0x8E) ? tempLockingValue + 1 : tempLockingValue;
					else
						lockingValue += tempLockingValue - 1;
				}
				else
				{
					lockingValue += tempLockingValue - 1;
				}
			}
		}
	}

	return lockingValue;
}

// std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
// std::vector<std::pair<std::vector<int>, int>>
template <class T>
inline void playAndPruneSortedMoves(Board & plateau, std::vector<std::pair<int, int>>& vSolution, std::vector<std::pair<std::vector<int>, T>>& vSortedMoves)
{
	auto itSortedMove = vSortedMoves.begin();
	std::vector<int> vTile = itSortedMove->first;
	do
	{
		vTile = itSortedMove->first;
		if (vTile.size() == 2)
		{
			plateau.RemovePairOfTiles(vTile[0], vTile[1]);
		}
		if (vTile.size() == 4)
		{
			plateau.RemovePairOfTiles(vTile[2], vTile[3]);
		}
		++itSortedMove;
	} while (
		itSortedMove != vSortedMoves.end() &&
		CheckIfLockedFromMove(plateau.getLogicalBoard(), plateau.getRemovedTilesMap(), vTile) &&
		plateau.TakeBack()
		);

	if (vTile.size() == 2)
	{
		vSolution.emplace_back(std::make_pair(vTile[0], vTile[1]));
	}
	if (vTile.size() == 4)
	{
		vSolution.emplace_back(std::make_pair(vTile[2], vTile[3]));
	}
}

// Max locking value to the rescue.
// Used when there is no choice meeting the requirements for the heuristic running.
// Section 3.3 from :
// https://iivq.net/scriptie/scriptie-bsc.pdf
inline bool tryMaxBlock(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, int>> vSortedMoves;
		auto mIndexToTile = plateau.getTilesMap();
		for (const auto& move : vMoves)
		{
			auto eval = EvalMoveMaxBlock(move, mIndexToTile);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
			[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
			{
				return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
			});

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline bool tryAlwaysFirst(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	while (!plateau.GetMovesLeft().empty())
	{
		auto itNextMove = plateau.GetMovesLeft().begin();
		vSolution.emplace_back(*itNextMove);
		plateau.RemovePairOfTiles(itNextMove->first, itNextMove->second);
	}
	return plateau.IsEmpty();
}

// Par nombre de dominos enlevés en une seule fois
// Par nombre de coups jouables
// Par max bloqueurs
inline bool tryRandomHeuristics(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto mOccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard);
			auto jouables = evalBruteForceOrderingEval.first;
			auto debloques = evalBruteForceOrderingEval.second;
			auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, mIndexToTile);
			vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock))); // 25
			//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock))); // 22
			//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques))); // 23
			//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, evalEvalMoveMaxBlock, jouables))); // 13
			//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(evalEvalMoveMaxBlock, debloques, jouables))); // 17
			//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(evalEvalMoveMaxBlock, jouables, debloques))); // 19
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
			[](const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& left, const std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>& right)
			{
				return
					left.first.size() > right.first.size() ||
					left.first.size() == right.first.size() && std::get<0>(left.second) > std::get<0>(right.second) ||
					(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) > std::get<1>(right.second)) ||
					(left.first.size() == right.first.size() && std::get<0>(left.second) == std::get<0>(right.second) && std::get<1>(left.second) == std::get<1>(right.second) && std::get<2>(left.second) > std::get<2>(right.second));
			});

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline uint8_t EvalMoveGreedy(
	const std::vector<int>& vMove,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::vector<int>& vWhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::vector<int> vWhatsLeftRemoved;
	std::map<Coordinates, int> mOccupationBoardRemoved;

	for (const auto& index : vMove)
	{
		RemoveTile(index,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			vWhatsLeft,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, vWhatsLeftRemoved, mOccupationBoardRemoved);
	}

	int removables = 0;
	for (const auto removable : arrRemovable)
		++removables;

	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : vWhatsLeftRemoved) vWhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return removables;
}

inline bool tryGreedy(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, int>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = EvalMoveGreedy(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, OccupationBoard);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
			[](const std::pair<std::vector<int>, int>& left, const std::pair<std::vector<int>, int>& right)
			{
				return left.second > right.second || (left.second == right.second && left.first.size() > right.first.size());
			});

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

// Removes 4 then 2
// Supposed to remove 2 out of three free tiles before 2 out of 2.
// Hence the "Pseudo".
inline bool tryPseudoMultipleFirst(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		std::vector<std::pair<std::vector<int>, int>> vSortedMoves;

		std::sort(vMoves.begin(), vMoves.end(), [](const std::vector<int>& left, const std::vector<int>& right) {return left.size() > right.size(); });

		for (const auto& move : vMoves)
			vSortedMoves.emplace_back(std::make_pair(move, move.size()));

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingPlayable(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, OccupationBoard);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
				return left.first.size() > right.first.size() || (left.first.size() == right.first.size() && left.second.first > right.second.first) ||
					((left.first.size() == right.first.size() && left.second.first == right.second.first && left.second.second > right.second.second));
			});

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingPlayablePadlocksFirst(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, OccupationBoard);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
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

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingFreed(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, OccupationBoard);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
			[](const std::pair<std::vector<int>, std::pair<int, int>>& left, const std::pair<std::vector<int>, std::pair<int, int>>& right)
			{
					return left.first.size() > right.first.size() || (left.first.size() == right.first.size() && left.second.second > right.second.second) ||
						((left.first.size() == right.first.size() && left.second.second == right.second.second && left.second.first > right.second.first));
			});

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

inline bool tryBruteForceOrderingFreedPadlocksFirst(Board plateau, std::vector<std::pair<int, int>>& vSolution)
{
	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);

	while (!vMoves.empty())
	{
		// Scooting step :
		std::vector<std::pair<std::vector<int>, std::pair<int, int>>> vSortedMoves;
		auto vLogicalBoard = plateau.getLogicalBoard();
		auto arrRemovable = plateau.getRemovable();
		auto mIndexToTile = plateau.getTilesMap();
		auto vWhatsLeft = plateau.getWhatsLeft();
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, OccupationBoard);
			vSortedMoves.emplace_back(std::make_pair(move, eval));
		}

		std::sort(vSortedMoves.begin(), vSortedMoves.end(),
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

		playAndPruneSortedMoves(plateau, vSolution, vSortedMoves);

		vMoves.clear();
		ConvertMovesToVector(plateau.GetMovesLeft(), vMoves);
	}
	return plateau.IsEmpty();
}

bool SolveRecThr(
	const std::vector<int> vMove,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::vector<int> vWhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>> &vSolution
#ifdef _DEBUG
	, uint64_t positions
#endif
)
{
	auto ret = SolveRecParallel(vMove, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution
#ifdef _DEBUG
		, positions
#endif
	);
	return ret;
}

// Just to work on a copy.
inline bool SolveRecInit(const Board& plateau,
	std::vector<std::pair<int, int>> vOldMoves,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::vector<int> vWhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& vSolution)
{
#ifdef _DEBUG
	std::vector<TileAndIndex> LogicalBoardRefForDebug = vLogicalBoard;
	std::array<bool, 144> RemovableRefForDebug = arrRemovable;
	std::map<int, Tile> mIndexToTileRefForDebug = mIndexToTile;
	std::vector<int> WhatsLeftRefForDebug = vWhatsLeft;
	std::map<Coordinates, int> mOccupationBoardRefForDebug = mOccupationBoard;
#endif


	std::vector<std::pair<bool (*)(Board plateau, std::vector<std::pair<int, int>>& vSolution), std::string>> vTries;

	vTries.push_back({ tryAlwaysFirst, "tryAlwaysFirst" });
	vTries.push_back({ tryRandomHeuristics, "tryRandomHeuristics" });
	vTries.push_back({ tryGreedy, "tryGreedy" });
	vTries.push_back({ tryPseudoMultipleFirst, "tryPseudoMultipleFirst" });
	vTries.push_back({ tryBruteForceOrderingPlayable, "tryBruteForceOrderingPlayable" });
	vTries.push_back({ tryBruteForceOrderingPlayablePadlocksFirst, "tryBruteForceOrderingPlayablePadlocksFirst" });
	vTries.push_back({ tryBruteForceOrderingFreed, "tryBruteForceOrderingFreed" });
	vTries.push_back({ tryBruteForceOrderingFreedPadlocksFirst, "tryBruteForceOrderingFreedPadlocksFirst" });
	vTries.push_back({ tryMaxBlock, "tryMaxBlock" });

	vSolution.clear();
	std::vector<std::pair<int, int>> vSolutionTemp;

	for (const auto& func : vTries)
	{
		if (func.first(plateau, vSolutionTemp))
		{
			vSolution.clear();
			vSolution = vSolutionTemp;
#ifdef _DEBUG
			std::cout << "vSolution : " << func.second << std::endl;
#else
			return true;
#endif
		}
		if (vSolution.size() < vSolutionTemp.size())
		{
			vSolution.clear();
			vSolution = vSolutionTemp;
		}
		vSolutionTemp.clear();
	}
#ifndef _DEBUG
	return true;
#endif
#ifdef _DEBUG
	for (const auto& move : vSolution)
		std::cout << move.first << ";" << move.second << std::endl;
#endif
	if (CheckIfLockedFromStart(mIndexToTile))
	{
#ifdef _DEBUG
		std::cout << "*************" << std::endl;
		std::cout << "*  Locked.  *" << std::endl;
		std::cout << "*************" << std::endl;
#endif
		return false;
	}

	return true;
	vSolution.clear();
	bool ret = false;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(vOldMoves, vMoves);

	std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
	for (const auto& move : vMoves)
	{
		auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard);
		auto jouables = evalBruteForceOrderingEval.first;
		auto debloques = evalBruteForceOrderingEval.second;
		auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, mIndexToTile);
		vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
		//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(debloques, jouables, evalEvalMoveMaxBlock)));
		//vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, evalEvalMoveMaxBlock, debloques)));
	}

	std::sort(vSortedMoves.begin(), vSortedMoves.end(),
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

	for (const auto& move : vSortedMoves)
	{
		ret = SolveRec(move.first, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution
#ifdef _DEBUG
			, positions
#endif
		);
		if (ret) break;
	}

#ifdef _DEBUG
	bool LogicalBoardOk = LogicalBoardRefForDebug.size() == vLogicalBoard.size();
	bool RemovableOk = RemovableRefForDebug.size() == arrRemovable.size();
	bool TilesMapOk = mIndexToTileRefForDebug.size() == mIndexToTile.size();
	bool WhatsLeftOk = WhatsLeftRefForDebug.size() == vWhatsLeft.size();
	bool mOccupationBoardOk = mOccupationBoardRefForDebug.size() == mOccupationBoard.size();
	for (auto& item : LogicalBoardRefForDebug) LogicalBoardOk &= vLogicalBoard.end() != std::find(vLogicalBoard.begin(), vLogicalBoard.end(), item);
	for (int i = 0; i < RemovableRefForDebug.size(); ++i) RemovableOk &= arrRemovable[i] == RemovableRefForDebug[i];
	for (auto& item : mIndexToTileRefForDebug) TilesMapOk &= mIndexToTile.contains(item.first) && mIndexToTile.find(item.first)->second == item.second;
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
	mTranspositionsTable.clear();

	return ret;
}

// Just to work on a copy.
// Note for this one : concurrence issues with the transposition table to prevent.
inline bool SolveRecInitAsync(const Board& plateau,
	std::vector<std::pair<int, int>> vOldMoves,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::vector<int> vWhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& vSolution)
{
	if (CheckIfLockedFromStart(mIndexToTile))
	{
		return false;
	}

	//return true;
	vSolution.clear();
	bool ret = false;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> vMoves;
	ConvertMovesToVector(vOldMoves, vMoves);

	std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
	for (const auto& move : vMoves)
	{
		auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard);
		auto jouables = evalBruteForceOrderingEval.first;
		auto debloques = evalBruteForceOrderingEval.second;
		auto evalEvalMoveMaxBlock = EvalMoveMaxBlock(move, mIndexToTile);
		vSortedMoves.emplace_back(std::make_pair(move, std::make_tuple(jouables, debloques, evalEvalMoveMaxBlock)));
	}

	std::sort(vSortedMoves.begin(), vSortedMoves.end(),
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


	/**/
	stopSolverNow = false;
	std::array < std::vector<std::pair<int, int>>, 2> vvSolutions;
	for (auto itMove = vSortedMoves.begin(); itMove != vSortedMoves.end();)
	{
		std::vector< std::future<bool>> vSolvers;
		for (int i = 0; i < 2; ++i)
		{
			if (itMove != vSortedMoves.end())
			{
				vSolvers.emplace_back(std::async(&SolveRecThr, itMove->first, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, std::ref(vvSolutions[i])
#ifdef _DEBUG
					, positions
#endif
				));
				++itMove;
			}
		}

		for (auto& solver : vSolvers)
		{
			auto retSolver = solver.get();
			ret |= retSolver;
		}
		if (ret) break;
	}
	/**/
	/*
	for (auto itMove = vSortedMoves.begin(); itMove != vSortedMoves.end();)
	{
		std::future<bool > solver = std::async(&SolveRecThr, itMove->first, vLogicalBoard, arrRemovable, mIndexToTile, vWhatsLeft, mOccupationBoard, vSolution
#ifdef _DEBUG
			, positions
#endif
		);
		ret |= solver.get();
		if (ret) break;
	}
	/**/

	// Transposition table must be cleared. The hash just means the positions has been seen at least once before. It doesn't mean it was in a lost game.
	// And the start position is always different.
	mTranspositionsTable.clear();

	return ret;
}
#ifdef _DEBUG
int64_t testAll(const Board& plateau)
{
	std::vector<std::pair<bool (*)(Board plateau, std::vector<std::pair<int, int>>& vSolution), std::string>> vTries;

	vTries.push_back({ tryAlwaysFirst, "tryAlwaysFirst" });
	vTries.push_back({ tryRandomHeuristics, "tryRandomHeuristics" });
	vTries.push_back({ tryGreedy, "tryGreedy" });
	vTries.push_back({ tryPseudoMultipleFirst, "tryPseudoMultipleFirst" });
	vTries.push_back({ tryBruteForceOrderingPlayable, "tryBruteForceOrderingPlayable" });
	vTries.push_back({ tryBruteForceOrderingPlayablePadlocksFirst, "tryBruteForceOrderingPlayablePadlocksFirst" });
	vTries.push_back({ tryBruteForceOrderingFreed, "tryBruteForceOrderingFreed" });
	vTries.push_back({ tryBruteForceOrderingFreedPadlocksFirst, "tryBruteForceOrderingFreedPadlocksFirst" });
	vTries.push_back({ tryMaxBlock, "tryMaxBlock" });


	std::vector<std::pair<int, int>> vSolutionTemp;

	uint64_t ret = 0ULL;
	for (const auto& func : vTries)
	{
		ret = ret << 7;
		ret |= func.first(plateau, vSolutionTemp) ? 1 : 0;
		vSolutionTemp.clear();
	}

	return ret;
}
#endif
