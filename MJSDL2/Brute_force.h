#pragma once
#include "Board.h"
#include <thread>
#include <future>
#ifndef BENCHMARK
#include <SDL.h>
#endif // !BENCHMARK

static std::atomic<bool> stopSolverNow;

static unsigned int processor_count;

static std::mutex mtxProcessorCount;

static std::mutex mtxStopNow;

inline unsigned int AskForCores(unsigned int wanted)
{
	mtxProcessorCount.lock();

	int allowed = 0;

	if (processor_count > 1)
	{
		allowed = std::min(processor_count, wanted);
		processor_count -= allowed;
	}
	else
	{
		allowed = 0;
	}

	mtxProcessorCount.unlock();

	return allowed;
}

inline void GiveCoresBack(unsigned int allowed)
{
	mtxProcessorCount.lock();

	processor_count += allowed;

	mtxProcessorCount.unlock();
}

inline bool SolveRecParallelInit(
	const std::vector<int> vMove,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::map<Coordinates, int> mOccupationBoard,
	std::map<int, int> arrGlobalOccurences,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t positions
#endif
);

inline uint8_t EvalMoveMaxBlock(
	const std::vector<int>& vMove,
	const std::map<int, Tile>& mIndexToTile);

inline bool CheckIfLockedFromMove(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToRemovedTile, const std::map<int, int>& arrGlobalOccurences, const std::vector<int>& vMove);

// Simplified version for the init.
// No need to restore anything.
inline void RemoveTile(
	const int index,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::map<Coordinates, int>& mOccupationBoard
)
{
	mIndexToTile.erase(index);

	std::vector<TileAndIndex>::iterator it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end() && it->Index != index; ++it);
	const auto& coord = arrIndexToCoord[it->Index];
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
		{
			auto index = mOccupationBoard[Coordinates(x + 1, y, z)];
			arrRemovable[index] = true;
		}
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x - 1, y, z)];
			arrRemovable[index] = true;
		}
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				arrRemovable[index] = true;
			}
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				arrRemovable[index] = true;
			}
		}
	}
}

inline void RemoveTile(
	const int index,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::map<Coordinates, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<TileAndIndex>& vLogicalBoardRemoved,
	std::vector<int>& vRemovableWasTrue,
	std::vector<int>& vRemovableWasFalse,
	std::map<int, Tile>& mIndexToRemovedTile,
	std::map<Coordinates, int>& mOccupationBoardRemoved
)
{
	mIndexToRemovedTile.emplace(index, mIndexToTile.find(index)->second);
	mIndexToTile.erase(index);

	std::vector<TileAndIndex>::iterator it = vLogicalBoard.begin();
	for (; it != vLogicalBoard.end() && it->Index != index; ++it);
	const auto& coord = arrIndexToCoord[it->Index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;

	vLogicalBoardRemoved.emplace_back(*it);
	vLogicalBoard.erase(it);
	mOccupationBoardRemoved[coord] = index;
	mOccupationBoard.erase(coord);

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
		const auto& domino = (*itFirst).TileObject;
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
		const auto& domino = (*itFirst).TileObject;
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
		tileTab[item.first] = uint64_t(item.second.Rank) + 1;
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
		const auto& boardDescriptionInHashtable = mTranspositionsTable[hash];
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
#ifdef BENCHMARK
		const auto maxSize = mTranspositionsTable.max_size();
		const auto currSize = mTranspositionsTable.size();
		if (mTranspositionsTable.max_size() <= mTranspositionsTable.size())
		{
			mtxStopNow.unlock();
			return true;
		}
#endif
		// I hope people will give up before the hash is really full...
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
		tileTab[item.first] = uint64_t(item.second.Rank) + 1;
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
		const auto& boardDescriptionInHashtable = mTranspositionsTable[hash];
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
	std::map<Coordinates, int>& mOccupationBoard)
{
	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::map<Coordinates, int> mOccupationBoardRemoved;
	bool full = false;
	for (const auto& index : vMove)
	{
		RemoveTile(index,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, mOccupationBoardRemoved);
	}

	auto moveEval = EvalMoves(vLogicalBoard, arrRemovable);

	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;

	return moveEval;
}

inline bool SolveRecParallel(
	const std::vector<int>& vMove,
	std::vector<TileAndIndex>& vLogicalBoard,
	std::array<bool, 144>& arrRemovable,
	std::map<int, Tile>& mIndexToTile,
	std::map<Coordinates, int>& mOccupationBoard,
	std::map<int, int> arrGlobalOccurences,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{
	int IndexesToRemoveFromSolutionCount = 0;
	std::vector<std::vector<int>> vNewMoves;

	std::vector<TileAndIndex> vLogicalBoardRemoved;
	std::vector<int> vRemovableWasTrue;
	std::vector<int> vRemovableWasFalse;
	std::map<int, Tile> mIndexToRemovedTile;
	std::map<Coordinates, int> mOccupationBoardRemoved;

	const auto pairing = mIndexToTile.find(*vMove.begin())->second.Pairing;
	arrGlobalOccurences[pairing] -= vMove.size();
	for (const auto& move : vMove)
	{
		RemoveTile(move,
			vLogicalBoard,
			arrRemovable,
			mIndexToTile,
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, mOccupationBoardRemoved);
	}
	vSolution.emplace_back(std::make_pair(vMove[0], vMove[1]));
	++IndexesToRemoveFromSolutionCount;
	if (vMove.size() == 4)
	{
		++IndexesToRemoveFromSolutionCount;
		vSolution.emplace_back(std::make_pair(vMove[2], vMove[3]));
	}

	bool loop = false;

	do
	{
		loop = false;
		vNewMoves.clear();
		SetMoves(vLogicalBoard, arrRemovable, vNewMoves);

		for (auto itMove = vNewMoves.begin(); itMove != vNewMoves.end(); )
		{
			const auto& vMove = *itMove;
			const auto index = *vMove.begin();
			const auto pairing = mIndexToTile.find(index)->second.Pairing;
			if (vMove.size() == arrGlobalOccurences.find(pairing)->second)
			{
				arrGlobalOccurences.erase(pairing);
				loop = true;
				for (const auto& move : vMove)
				{
					RemoveTile(move,
						vLogicalBoard,
						arrRemovable,
						mIndexToTile,
						mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, mOccupationBoardRemoved);
				}
				++IndexesToRemoveFromSolutionCount;
				vSolution.emplace_back(std::make_pair(vMove[0], vMove[1]));
				if (vMove.size() == 4)
				{
					++IndexesToRemoveFromSolutionCount;
					vSolution.emplace_back(std::make_pair(vMove[2], vMove[3]));
				}

				itMove = vNewMoves.erase(itMove);
			}
			else
			{
				++itMove;
			}
		}
	} while (loop);

	auto ret = vLogicalBoard.empty();
	if (!ret && !stopSolverNow)
	{
		if (!CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, arrGlobalOccurences, vMove) && !vNewMoves.empty() && !stopSolverNow)
		{
			// For now there is only one async running...
			if (!stopNowParallel(mIndexToTile
#ifdef _DEBUG
				, positions
#endif
			))
			{
				std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
				for (const auto& move : vNewMoves)
				{
					auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard);
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
				const auto allowed_processor_count = AskForCores(vSortedMoves.size());
				if (allowed_processor_count > 1)
				{
					auto arrSolutions = std::make_unique<std::vector<std::pair<int, int>>[]>((unsigned int)allowed_processor_count);
					const auto maxthread = std::min((unsigned int)vSortedMoves.size(), (unsigned int)allowed_processor_count);

					for (auto itMove = vSortedMoves.begin(); itMove != vSortedMoves.end();)
					{
						std::vector< std::future<bool>> vSolvers;
						for (int i = 0; i < maxthread; ++i)
						{
							if (itMove != vSortedMoves.end())
							{
								vSolvers.emplace_back(std::async(&SolveRecParallelInit, itMove->first, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, arrGlobalOccurences, std::ref(arrSolutions[i])
#ifdef _DEBUG
									, positions
#endif
								));
								++itMove;
							}
						}

						int i = 0;
						for (auto& solver : vSolvers)
						{
							if (solver.get())
							{
								stopSolverNow = true;
								// Funny case when more than one found a solution...
								if (!ret)
									vSolution.insert(vSolution.end(), arrSolutions[i].begin(), arrSolutions[i].end());
								ret = true;
							}
							i++;
						}

						if (stopSolverNow)
							break;
					}

					GiveCoresBack(allowed_processor_count);
				}
				else
				{
					for (const auto& move : vSortedMoves)
					{
						ret = SolveRecParallel(move.first, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, arrGlobalOccurences, vSolution
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
		}
		if (!ret)
		{
			for (int i = 0; i < IndexesToRemoveFromSolutionCount; ++i)
				vSolution.pop_back();
		}
	}
	/**/
	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
	for (auto& item : mOccupationBoardRemoved) mOccupationBoard[item.first] = item.second;
	/**/

	return ret;
}

inline bool CheckIfLockedFromMove(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToRemovedTile, const std::map<int, int>& arrGlobalOccurences, const std::vector<int>& vMove)
{
	if (vMove.size() == 4)
		return false;

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
		const auto& c1 = vPairedIndex[0];
		const auto& c2 = vPairedIndex[1];
		if (c1.X == c2.X && c1.Y == c2.Y && c1.DecX == c2.DecX && c1.DecY == c2.DecY)
			return true;

		if (c1.Y == c2.Y && c1.Z == c2.Z) // Same plane, same row
		{
			int z = c1.Z;
			int y = c1.Y;
			std::array<std::array<std::array<TileAndIndex, 4>, 8>, 12> arrBoard;

			std::map<int, int> arrRowOccurences {};

			auto beginning = -1;
			auto end = -1;
			auto leftPadlockPairing = -1;
			auto rightPadlockPairing = -1;
			auto rightRightPadlockPairing = -1;
			std::map<int, int> mPairingCount;
			std::map<int, int> mPairingFirst;
			std::map<int, int> mPairingLast;

			for (const auto& tileAndIndex : vLogicalBoard)
			{
				if (tileAndIndex.Index < 140)
					arrBoard[tileAndIndex.X][tileAndIndex.Y][tileAndIndex.Z] = tileAndIndex;
				if (tileAndIndex.Y == c1.Y && tileAndIndex.Z == c1.Z) // Same plane, same row
				{
					++arrRowOccurences[tileAndIndex.TileObject.Pairing];
					if (beginning == -1)
						beginning = tileAndIndex.X;
					beginning = std::min(beginning, tileAndIndex.X);
					end = std::max(end, tileAndIndex.X);
				}
				if (3 <= y && y <= 4 && z == 0)
				{
					if (tileAndIndex.Index == 0x8C)
					{
						leftPadlockPairing = tileAndIndex.TileObject.Pairing;

						// Fake entries for the padlocks.
						++mPairingCount[leftPadlockPairing];
						mPairingFirst[leftPadlockPairing] = -1;
					}
					else if (tileAndIndex.Index == 0x8D)
					{
						rightPadlockPairing = tileAndIndex.TileObject.Pairing;

						// Fake entries for the padlocks.
						++mPairingCount[rightPadlockPairing];
						mPairingLast[rightPadlockPairing] = 12;
					}
					else if (tileAndIndex.Index == 0x8E)
					{
						rightRightPadlockPairing = tileAndIndex.TileObject.Pairing;

						// Fake entries for the padlocks.
						++mPairingCount[rightRightPadlockPairing];
						mPairingLast[rightRightPadlockPairing] = 13;
					}
				}
			}

			// First pass : check potential issues
			// 
			// For 3 identical tiles :
			// AAABB, AABAB but not ABAAB!
			// BBAAA, BABAA but not BAABA!
			auto horizontalLimits = std::make_pair(std::max(0, beginning), std::min(11, end)); // Just to re-use some code...

			for (int x = horizontalLimits.first; x <= horizontalLimits.second; ++x)
			{
				// First get the tile.
				const auto& object = arrBoard[x][y][z].TileObject;
				// Get the first occurence
				if (!mPairingFirst.contains(object.Pairing))
					mPairingFirst[object.Pairing] = x;
				// Get the last occurence
				if (!mPairingLast.contains(object.Pairing) || mPairingLast.find(object.Pairing)->second < x) // Because of the left and right blocker fake init before...
					mPairingLast[object.Pairing] = x;
				// Get the count of occurences
				++mPairingCount[object.Pairing];
			}

			// shouldn't be an issue because of the trimming right after but still...
			if (!mPairingLast.contains(leftPadlockPairing))
				mPairingLast[leftPadlockPairing] = -1;

			if (!mPairingFirst.contains(rightPadlockPairing))
				mPairingFirst[leftPadlockPairing] = 12;

			if (!mPairingFirst.contains(leftPadlockPairing))
				mPairingFirst[rightRightPadlockPairing] = 13;

			/* TO DO                                        */
			/* Cross lockings                               */
			/* Can't be done after because of the pondering */
			/* TO DO                                        */

			// Ponder
			// Conditionnal pondering : may miss some lockings but it's better than fake ones.
			if (3 <= y && y <= 4 && z == 0 && leftPadlockPairing != -1 && rightPadlockPairing != -1)
			{
				int yPonder = y == 3 ? 4 : 3;
				for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
				{
					// First get the tile.
					const auto pairing = arrBoard[x][yPonder][0].TileObject.Pairing;
					if (
						(leftPadlockPairing != -1 && pairing == leftPadlockPairing) ||
						(rightPadlockPairing != -1 && pairing == rightPadlockPairing) ||
						(rightRightPadlockPairing != -1 && pairing == rightRightPadlockPairing))
						++mPairingCount[pairing];
				}
			}

			// Remove the useless ones :
			for (const auto& item : mPairingCount)
			{
				// Actually, I realized that AAA/BBB is an issue. AABABB, AABBAB, ABAABB, AABABB but not AAABBB. I'll try to think about something.
				if (item.second != arrGlobalOccurences.find(item.first)->second)
				{
					mPairingFirst.erase(item.first);
					mPairingLast.erase(item.first);
				}
			}

			// There's at least one because that was the condition to be here in the first place.
			// But one's not enough to have an issue...
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
							return true;
						// fBx < fAx && lBx < lAx
						/*if (it->second > itNext->second && mPairingLast.find(it->first)->second > mPairingLast.find(itNext->first)->second)
							return true;*/
					}
				}
			}
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
inline bool CheckIfLockedFromStart(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToTile, int* cause = NULL)
{
	// Index -> TileObject
	// std::map<int, Tile>& mIndexToTile

	if (mIndexToTile.size() < 144)
		return false;

	std::array<std::array<std::array<TileAndIndex, 4>, 8>, 12> arrBoard;

	for (const auto& tileAndIndex : vLogicalBoard)
	{
		if (tileAndIndex.Index < 140)
			arrBoard[tileAndIndex.X][tileAndIndex.Y][tileAndIndex.Z] = tileAndIndex;
	}

	std::vector<int> vStartPos;
	vStartPos.emplace_back(0x88);
	vStartPos.emplace_back(0x89);
	vStartPos.emplace_back(0x8A);
	vStartPos.emplace_back(0x8B);

	/**/
	// Blocked by the centerPadlock.
	auto centerPadlock = mIndexToTile.find(0x8F)->second.Pairing;
	int pairs = 0;
	for (auto itIndex = vStartPos.begin(); itIndex != vStartPos.end(); ++itIndex)
	{
		auto c = arrIndexToBoardCoord[*itIndex];
		auto x = std::get<0>(c);
		auto y = std::get<1>(c);

		int currPairs = 0;

		for (int z = 0; z <= std::get<2>(c); ++z)
		{
			if (centerPadlock == arrBoard[x][y][z].TileObject.Pairing) ++pairs, ++currPairs;
			// If there are 3 others under the centerPadlock, game is alreay over.
			if (pairs == 3)
			{
				if (cause != NULL) { *cause = 1; };
				return true;
			}
			// Pure vertical lock.
			// Removing the centerPadlock will never unlock the last twos.
			if (currPairs == 2)
			{
				if (cause != NULL) { *cause = 2; };
				return true;
			}
		}
	}

	std::vector<TileAndIndex> vStartPosTileIndex;
	for (int z = 3; z >= 2; --z)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto horizontalLimits = arrHorizontalLimits[y][z];
			for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
				vStartPosTileIndex.emplace_back(arrBoard[x][y][z]);
		}
	}

	for (auto itIndex = vStartPosTileIndex.begin(); itIndex != vStartPosTileIndex.end(); ++itIndex)
	{
		auto x = itIndex->X;
		auto y = itIndex->Y;

		auto firstPadlock = itIndex->TileObject.Pairing;
		int pairs = 0;
		// Pure vertical lock.
		for (int z = 0; z < itIndex->Z; ++z)
		{
			if (firstPadlock == arrBoard[x][y][z].TileObject.Pairing) ++pairs;
			// Removing the first will never unlock the last twos.
			if (pairs == 2)
			{
				if (cause != NULL) { *cause = 3; };
				return true;
			}
		}
	}

	vStartPosTileIndex.clear();
	for (int z = 3, y = 0; y < 8; ++y)
	{
		auto horizontalLimits = arrHorizontalLimits[y][z];
		for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
			vStartPosTileIndex.emplace_back(arrBoard[x][y][z]);
	}
	for (auto itIndex = vStartPosTileIndex.begin(); itIndex != vStartPosTileIndex.end(); ++itIndex)
	{
		// Crossed vertical lock.
		/*
			AB
			AA
			BA
			BB
		*/
		auto itRef = itIndex;
		auto refAppairage = itRef->TileObject.Pairing;
		int refX = itRef->X;
		int refY = itRef->Y;
		int refZ = itRef->Z;
		for (auto itSecond = itRef + 1; itSecond != vStartPosTileIndex.end(); ++itSecond)
		{
			auto secondAppairage = itSecond->TileObject.Pairing;
			int secondX = itSecond->X;
			int secondY = itSecond->Y;
			int secondZ = itSecond->Z;
			auto refCount = 1;
			auto secondCount = 1;
			for (int z = 0; z < 3; ++z)
			{
				auto compAppairage = arrBoard[refX][refY][z].TileObject.Pairing;
				if (refAppairage == compAppairage) ++refCount;
				if (secondAppairage == compAppairage) ++secondCount;

				compAppairage = arrBoard[secondX][secondY][z].TileObject.Pairing;
				if (refAppairage == compAppairage) ++refCount;
				if (secondAppairage == compAppairage) ++secondCount;
			}
			if (secondCount == 4 && refCount == secondCount)
			{
				if (cause != NULL) { *cause = 4; };
				return true;
			}
		}
	}

	// For left and right padlocks : funny blocking...
	// Doesn't work if not a starting pos. There could be padlocks missing.
	// First pass : check potential issues
	std::map<int, int> mPairingCount;
	std::map<int, int> mPairingFirst;
	std::map<int, int> mPairingLast;

	auto leftPadlockPairing = mIndexToTile.find(0x8C)->second.Pairing;
	auto rightPadlockPairing = mIndexToTile.find(0x8D)->second.Pairing;
	auto rightRightPadlockPairing = mIndexToTile.find(0x8E)->second.Pairing;

	// Creating a fake 27 tiles line to include the padlocks obstruction from the beginning to the end.
	std::vector<int> row;
	row.emplace_back(leftPadlockPairing);
	for (int x = 0; x < 12; ++x)
		row.emplace_back(arrBoard[x][3][0].TileObject.Pairing);
	for (int x = 0; x < 12; ++x)
		row.emplace_back(arrBoard[x][4][0].TileObject.Pairing);
	row.emplace_back(rightPadlockPairing);
	row.emplace_back(rightRightPadlockPairing);

	for (int x = 0; x < row.size(); ++x)
	{
		// Get the first occurence
		if (!mPairingFirst.contains(row[x]))
			mPairingFirst[row[x]] = x;
		// Get the last occurence
		mPairingLast[row[x]] = x;
		// Get the count of occurences
		++mPairingCount[row[x]];
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

	// Specific to padlocks. They have to be involved or the test is not relevant.
	if (mPairingFirst.contains(0x8C) || mPairingFirst.contains(0x8D) || mPairingFirst.contains(0x8E))
	{
		if (mPairingFirst.size() > 1) // Ok, now we have an issue...
		{
			auto itEnd = mPairingFirst.end();
			--itEnd;
			for (auto it = mPairingFirst.begin(); it != itEnd; ++it)
			{
				// Specific to padlocks. They have to be involved or the test is not relevant.
				if(it->first == 0x8C || it->first == 0x8D || it->first == 0x8E)
				{
					auto itNext = it;
					for (++itNext; itNext != mPairingFirst.end(); ++itNext)
					{
						// fAx < fBx && lAx < lBx
						if (it->second < itNext->second && mPairingLast.find(it->first)->second < mPairingLast.find(itNext->first)->second)
						{
							if (cause != NULL) { *cause = 5; };
							return true;
						}
						// fBx < fAx && lBx < lAx
						if (it->second > itNext->second && mPairingLast.find(it->first)->second > mPairingLast.find(itNext->first)->second)
						{
							if (cause != NULL) { *cause = 5; };
							return true;
						}
					}
				}
			}
		}
	}

	// There can't be holes in the lines.
	for (int z = 3; z >= 0; --z)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto horizontalLimits = arrHorizontalLimits[y][z];
			// You need at least 8 slots to block 8 tiles...
			if (horizontalLimits.second - horizontalLimits.first + 1 > 7) // No need to check for y = 3 or y = 5
			{
				// First pass : check potential issues
				mPairingCount.clear();
				mPairingFirst.clear();
				mPairingLast.clear();

				std::vector<int> row;

				if (3 <= y && y <= 4 && z == 0)
				{
					row.emplace_back(leftPadlockPairing);
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
						row.emplace_back(arrBoard[x][y][z].TileObject.Pairing);
					row.emplace_back(rightPadlockPairing);


					// Ponder
					int yPonder = y == 3 ? 4 : 3;
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
					{
						// First get the tile.
						const auto pairing = arrBoard[x][yPonder][0].TileObject.Pairing;
						if (pairing == leftPadlockPairing)
							row.insert(row.begin(), leftPadlockPairing);
					}
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
					{
						// First get the tile.
						const auto pairing = arrBoard[x][yPonder][0].TileObject.Pairing;
						if (pairing == rightPadlockPairing)
							row.emplace_back(rightPadlockPairing);
					}
					
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
					{
						// First get the tile.
						const auto pairing = arrBoard[x][yPonder][0].TileObject.Pairing;
						if (pairing == rightRightPadlockPairing)
							row.emplace_back(rightRightPadlockPairing);
					}
					row.emplace_back(rightRightPadlockPairing);

				}
				else
				{
					for (int x = std::max(0, horizontalLimits.first); x <= horizontalLimits.second; ++x)
						row.emplace_back(arrBoard[x][y][z].TileObject.Pairing);
				}

				for (int x = 0; x < row.size(); ++x)
				{
					// Get the first occurence
					if (!mPairingFirst.contains(row[x]))
						mPairingFirst[row[x]] = x;
					// Get the last occurence
					mPairingLast[row[x]] = x;
					// Get the count of occurences
					++mPairingCount[row[x]];
				}

				// Remove the useless ones :
				for (const auto& item : mPairingCount)
				{
					// Actually, I realized that AAA || BBB can be an issue.
					// With 3 of each :
					// ABBA BAAB : ok.
					// AABB BBAA ABAB BABA : always locked
					// I'll try to think about something.
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
							{
								if (cause != NULL) { *cause = 6; };
								return true;
							}
							// fBx < fAx && lBx < lAx
							/*
							if (it->second > itNext->second && mPairingLast.find(it->first)->second > mPairingLast.find(itNext->first)->second)
							{
								if (cause != NULL) { *cause = 7; };
								return true;
							}
							*/
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
	const std::map<int, Tile>& mIndexToTile)
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
			for (int x = 11; x >= 0 && (mIndexToTile.contains(arrBoardCoordToIndex[x][3][0])); --x, ++tempLockingValue);
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

inline bool CheckIfLockedFromMove(const std::vector<TileAndIndex>& vLogicalBoard, const std::map<int, Tile>& mIndexToRemovedTile, const std::vector<int>& vMove)
{
	std::map<int, int> arrGlobalOccurences {};
	for (const auto& tileAndIndex : vLogicalBoard)
		++arrGlobalOccurences[tileAndIndex.TileObject.Pairing];

	return CheckIfLockedFromMove(vLogicalBoard, mIndexToRemovedTile, arrGlobalOccurences, vMove);
}

// std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
// std::vector<std::pair<std::vector<int>, int>>
template <class T>
inline void playAndPruneSortedMoves(Board& plateau, std::vector<std::pair<int, int>>& vSolution, std::vector<std::pair<std::vector<int>, T>>& vSortedMoves)
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
		const auto& mIndexToTile = plateau.getTilesMap();
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
		auto mOccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard);
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
			mOccupationBoard, vLogicalBoardRemoved, vRemovableWasTrue, vRemovableWasFalse, mIndexToRemovedTile, mOccupationBoardRemoved);
	}

	int removables = 0;
	for (const auto removable : arrRemovable)
		++removables;

	for (auto& item : vLogicalBoardRemoved) vLogicalBoard.emplace_back(item);
	for (auto& item : vRemovableWasTrue) arrRemovable[item] = true;
	for (auto& item : vRemovableWasFalse) arrRemovable[item] = false;
	for (auto& item : mIndexToRemovedTile) mIndexToTile.emplace(item.first, item.second);
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
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = EvalMoveGreedy(move, vLogicalBoard, arrRemovable, mIndexToTile, OccupationBoard);
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
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, OccupationBoard);
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
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, OccupationBoard);
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
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, OccupationBoard);
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
		auto OccupationBoard = plateau.getOccupationBoard();
		for (const auto& move : vMoves)
		{
			auto eval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, OccupationBoard);
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

// Just to work on a copy.
inline bool SolveRecParallelInit(
	const std::vector<int> vMove,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::map<Coordinates, int> mOccupationBoard,
	std::map<int, int> arrGlobalOccurences,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t positions
#endif
)
{
	return SolveRecParallel(vMove, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, arrGlobalOccurences, vSolution
#ifdef _DEBUG
		, positions
#endif
	);
}

// Just to work on a copy.
inline bool SolveRecAsyncInit(
	std::vector<std::pair<int, int>> vOldMoves,
	std::vector<TileAndIndex> vLogicalBoard,
	std::array<bool, 144> arrRemovable,
	std::map<int, Tile> mIndexToTile,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& vSolution
#ifdef _DEBUG
	, uint64_t positions
#endif
)
{

	bool ret = false;

	int lockStatus = 0;
	if (CheckIfLockedFromStart(vLogicalBoard, mIndexToTile, &lockStatus))
	{
		stopSolverNow = true;
#ifdef _DEBUG
		std::cout << "*************" << std::endl;
		std::cout << "*  Locked.  *" << std::endl;
		std::cout << "*     " << lockStatus << "     *" << std::endl;
		std::cout << "*************" << std::endl;
#endif
}
	else
	{
		stopSolverNow = false;
		// I keep 2 for the main thread and this one.
		processor_count = std::thread::hardware_concurrency();

		// May return 0 when not able to detect.
		processor_count = processor_count >= 2 ? processor_count - 2 : 0;

		vSolution.clear();

		// New move container to remove the tiles 2 at once or 4 at once.
		std::vector<std::vector<int>> vMoves;
		ConvertMovesToVector(vOldMoves, vMoves);

		bool loop = false;

		std::map<int, int> arrGlobalOccurences {};
		for (const auto& tileAndIndex : vLogicalBoard)
			++arrGlobalOccurences[tileAndIndex.TileObject.Pairing];

		do
		{
			loop = false;
			vMoves.clear();
			SetMoves(vLogicalBoard, arrRemovable, vMoves);

			for (auto itMove = vMoves.begin(); itMove != vMoves.end();)
			{
				const auto& vMove = *itMove;
				const auto index = *vMove.begin();
				const auto pairing = mIndexToTile.find(index)->second.Pairing;
				if (vMove.size() == arrGlobalOccurences.find(pairing)->second)
				{
					arrGlobalOccurences.erase(pairing);
					loop = true;

					for (const auto& move : vMove)
						RemoveTile(move, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard);

					vSolution.emplace_back(std::make_pair(vMove[0], vMove[1]));

					if (vMove.size() == 4)
						vSolution.emplace_back(std::make_pair(vMove[2], vMove[3]));

					itMove = vMoves.erase(itMove);
				}
				else
				{
					++itMove;
				}
			}
		} while (loop);

		if (!stopSolverNow)
		{
			std::vector<std::pair<std::vector<int>, std::tuple<int, int, uint8_t>>> vSortedMoves;
			for (const auto& move : vMoves)
			{
				auto evalBruteForceOrderingEval = BruteForceOrderingEval(move, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard);
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
			const auto allowed_processor_count = AskForCores(vSortedMoves.size());
			if (allowed_processor_count > 1)
			{
				auto arrSolutions = std::make_unique<std::vector<std::pair<int, int>>[]>((unsigned int)allowed_processor_count);
				const auto maxthread = std::min((unsigned int)vSortedMoves.size(), (unsigned int)allowed_processor_count);

				for (auto itMove = vSortedMoves.begin(); itMove != vSortedMoves.end();)
				{
					std::vector< std::future<bool>> vSolvers;
					for (int i = 0; i < maxthread; ++i)
					{
						if (itMove != vSortedMoves.end())
						{
							vSolvers.emplace_back(std::async(&SolveRecParallelInit, itMove->first, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, arrGlobalOccurences, std::ref(arrSolutions[i])
#ifdef _DEBUG
								, positions
#endif
							));
							++itMove;
					}
				}

					int i = 0;
					for (auto& solver : vSolvers)
					{
						if (solver.get())
						{
							stopSolverNow = true;
							// Funny case when more than one thread found a solution...
							if (!ret)
								vSolution.insert(vSolution.end(), arrSolutions[i].begin(), arrSolutions[i].end());
							ret = true;
						}
						i++;
					}
					if (ret)
						break;
					if (stopSolverNow)
						break;
			}

				GiveCoresBack(allowed_processor_count);
		}
			else
			{
				for (const auto& move : vSortedMoves)
				{
					ret = SolveRecParallel(move.first, vLogicalBoard, arrRemovable, mIndexToTile, mOccupationBoard, arrGlobalOccurences, vSolution
#ifdef _DEBUG
						, positions
#endif
					);
					if (ret)
						break;
					if (stopSolverNow)
						break;
			}
	}

			mTranspositionsTable.clear();
		}
	}

	if (!ret)
		vSolution.clear();

#ifndef BENCHMARK
	SDL_Event event;

	event.type = SDL_USEREVENT;
	event.user.code = ret ? 1 : 0;
	event.user.data1 = NULL;
	event.user.data2 = NULL;
	SDL_PushEvent(&event);
#endif
	return ret;
		}

// Just to work on a copy.
inline bool TryHeuristics(const Board& plateau,
	std::vector<TileAndIndex> vLogicalBoard,
	std::map<int, Tile> mIndexToTile,
	std::vector<std::pair<int, int>>& vSolution)
{
	int lockStatus = 0;

	if (CheckIfLockedFromStart(vLogicalBoard, mIndexToTile, &lockStatus))
	{
#ifdef _DEBUG
		std::cout << "*************" << std::endl;
		std::cout << "*  Locked.  *" << std::endl;
		std::cout << "*     " << lockStatus << "     *" << std::endl;
		std::cout << "*************" << std::endl;
#endif
#ifndef _DEBUG
		return false;
#endif
}

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

	bool bSolutionFound = false;

	for (const auto& func : vTries)
	{
		if (func.first(plateau, vSolutionTemp))
		{
			vSolution.clear();
			vSolution = vSolutionTemp;
			bSolutionFound = true;
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

#ifdef _DEBUG
	for (const auto& move : vSolution)
		std::cout << move.first << ";" << move.second << std::endl;
#endif
	if (bSolutionFound)
		return true;

	return false;
}

#if defined(_DEBUG) || defined(BENCHMARK)
inline bool testAll(const Board& plateau, std::array <unsigned int, 9>& arrResult)
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

	bool ret = false;
	int i = 0;
	for (const auto& func : vTries)
	{
		const auto tempRet = func.first(plateau, vSolutionTemp);
		ret |= tempRet;
		if(tempRet)
			arrResult[i] = 1;
		vSolutionTemp.clear();
		++i;
	}

	return ret;
	}
#endif
