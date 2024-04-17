#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>

class Board
{
private:
	static bool CompLogicalBoardDownLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardUpLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardUpRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardDownRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompRemovableBoard(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);

public:
	std::vector<std::pair<int, int>> Solution;
	Board();
	void InitBoard();
	const int getDominoFromIndex(const int index) { return TilesMap[index]; }
	const bool getRemovableFromIndex(const int index) { return Removable[index]; }
	bool RemovePairOfTiles(const int, const int);
	const bool IsBlocked() { return Moves.size() == 0; }
	const int HowManyMovesLeft() { return Moves.size(); }
	const std::vector<std::pair<int, int>>& GetMovesLeft() { return Moves; }
	bool IsEmpty() { return WhatsLeft.empty(); }
	int getNumberOfTilesLeft() { return WhatsLeft.size(); }
	const std::vector<int>& getWhatsLeft() { return WhatsLeft; }
	const std::vector<std::tuple<double, double, double, int, int>>& getLogicalBoard() { return LogicalBoard; }
	void SortBoard(const uint8_t direction);
	bool Solve();
	
private:
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft; // Index
	std::map<int, int> TilesMap; // index -> domino
	std::map<std::tuple<double, double, double>, int> mOccupationBoard; // (x, y, z) -> index
	std::array<std::tuple<double, double, double>, 144> InitIndexToCoord;
	std::vector<std::tuple<double, double, double, int, int>> LogicalBoard; // (x, y, z, domino, index)
	std::array<bool, 144> Removable = {
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false, false, false, false, false, false, false,
		false, false, false, false
	};
	void RemoveTile(int);
	void BuildMoves(std::vector<std::tuple<double, double, double, int, int>>& RemovableBoard, std::vector<std::tuple<double, double, double, int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves);
	std::vector<std::pair<int, int>> Moves;
	void SetMoves();
};

inline void RemoveTile(
	const int index,
	std::vector<std::tuple<double, double, double, int, int>>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<std::tuple<double, double, double>, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<std::tuple<double, double, double, int, int>>& LogicalBoardBack,
	std::vector<int>& RemovableFalseBack,
	std::vector<int>& RemovableTrueBack,
	std::map<int, int>& TilesMapBack,
	std::vector<int>& WhatsLeftBack,
	std::map<std::tuple<double, double, double>, int>& mOccupationBoardBack
	)
{
	TilesMapBack[index] = TilesMap[index];
	TilesMap.erase(index);

	std::vector<std::tuple<double, double, double, int, int>>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && std::get<4>(*it) != index; ++it);
	double x = std::get<0>(*it);
	double y = std::get<1>(*it);
	double z = std::get<2>(*it);
	LogicalBoardBack.emplace_back(*it);
	LogicalBoard.erase(it);
	mOccupationBoardBack[std::make_tuple(x, y, z)] = index;
	mOccupationBoard.erase(std::make_tuple(x, y, z));

	auto itWL = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (itWL != WhatsLeft.end())
	{
		WhatsLeftBack.emplace_back(*itWL);
		WhatsLeft.erase(itWL);
	}

	RemovableFalseBack.emplace_back(index);
	Removable[index] = false;

	if (index == 0x8F)
	{
		Removable[0x88] = true;
		Removable[0x89] = true;
		Removable[0x8A] = true;
		Removable[0x8B] = true;

		RemovableTrueBack.emplace_back(0x88);
		RemovableTrueBack.emplace_back(0x89);
		RemovableTrueBack.emplace_back(0x8A);
		RemovableTrueBack.emplace_back(0x8B);
	}
	else if (index == 0x8C)
	{
		Removable[0x1E] = true;
		Removable[0x2A] = true;
		RemovableTrueBack.emplace_back(0x1E);
		RemovableTrueBack.emplace_back(0x2A);
	}
	else if (index == 0x8E)
	{
		Removable[0x8D] = true;
		RemovableTrueBack.emplace_back(0x8D);
	}
	else if (index == 0x8D)
	{
		Removable[0x29] = true;
		RemovableTrueBack.emplace_back(0x29);
		Removable[0x35] = true;
		RemovableTrueBack.emplace_back(0x35);
	}
	else
	{
		if (x < 11 && mOccupationBoard.contains(std::make_tuple(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x + 1, y, z + 1))))
		{
			auto index = mOccupationBoard[std::make_tuple(x + 1, y, z)];
			Removable[index] = true;
			RemovableTrueBack.emplace_back(index);
		}
		if (x > 0 && mOccupationBoard.contains(std::make_tuple(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[std::make_tuple(x - 1, y, z)];
			Removable[index] = true;
			RemovableTrueBack.emplace_back(index);
		}
		if (z > 0) // mOccupationBoard[std::make_tuple(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(std::make_tuple(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[std::make_tuple(x, y, z - 1)];
				Removable[index] = true;
				RemovableTrueBack.emplace_back(index);
			}
			if (x > 0 && !mOccupationBoard.contains(std::make_tuple(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[std::make_tuple(x, y, z - 1)];
				Removable[index] = true;
				RemovableTrueBack.emplace_back(index);
			}
		}
	}
}

inline void BuildMoves(std::vector<std::tuple<double, double, double, int, int>>& RemovableBoard, std::vector<std::tuple<double, double, double, int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves)
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

inline void SetMoves(std::vector<std::tuple<double, double, double, int, int>>& LogicalBoard, std::array<bool, 144>& Removable, std::vector<std::pair<int, int>>& Moves)
{
	std::vector<std::tuple<double, double, double, int, int>> RemovableBoard; // (x, y, z, domino, index)
	for (const auto& tuple : LogicalBoard)
	{
		if (Removable[std::get<4>(tuple)]) RemovableBoard.emplace_back(tuple);
	}

	Moves.clear();
	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right)
		{
			return std::get<3>(left) < std::get<3>(right);
		});
	auto itFirst = RemovableBoard.begin();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

static std::map<uint64_t, std::array<uint64_t, 144 / 8>> hashtable;

// Pseudo-hash.
// Collision rate to check.
inline uint64_t getHash(std::vector<std::pair<int, int>>& Moves,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap
	)
{
	// 64 56 48 40 32 24 16 8
	uint64_t indexsum = 0ULL; // 14 : 14 + 8 + 7 + 8 + 13
	uint64_t tilesum = 0ULL; // 14 : 8 + 7 + 8 + 13
	uint64_t removables = 0ULL; // 8 : 7 + 8 + 13
	uint64_t nmoves = 0ULL; // 7 : 8 + 13
	uint64_t count = 0ULL; // 8 : 13

	uint64_t alternateindexsum = 0ULL;
	uint64_t alternatetilesum = 0ULL;
	int pair = true;
	for (auto& item : TilesMap)
	{
		indexsum += item.first;
		tilesum += item.second;
		++count;
		alternateindexsum = pair ? item.first : 3 * item.first;
		alternatetilesum = pair ? item.second : 3 * item.second;
	}

	for (auto& item : Removable) if (item) ++removables;

	nmoves = Moves.size(); // 7

	auto rIndex = alternateindexsum % 10; // 7
	auto rTile = alternatetilesum % 5; // 6
	rIndex = rIndex == 0 ? 0 : 10 - rIndex;
	rTile = rTile == 0 ? 0 : 5 - rTile;

	uint64_t hash = indexsum << (14 + 8 + 7 + 8 + 13) | tilesum << (8 + 7 + 8 + 13) | removables << (7 + 8 + 13) | nmoves << (8 + 13) | count << (13) | alternateindexsum << 6 | alternatetilesum;
	
	return hash;
}

inline bool stopNow(std::vector<std::pair<int, int>>& Moves,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap
)
{
	auto hash = getHash(Moves, Removable, TilesMap);

	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = item.second;
	}

	std::array<uint64_t, 144 / 8> boardDescription;

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
		boardDescription[i / 8] = temp;
	}

	if (hashtable.contains(hash))
	{
		auto boardDescriptionInHashtable = hashtable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
				std::cout << "Collision de hash" << std::endl;
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

inline bool SolveRec(
	std::pair<int, int> & Move,
	int _index,
	std::vector<std::pair<int, int>> & Moves,
	std::vector<std::tuple<double, double, double, int, int>> & LogicalBoard,
	std::array<bool, 144> & Removable,
	std::map<int, int> & TilesMap,
	std::vector<int> & WhatsLeft,
	std::map<std::tuple<double, double, double>, int> & mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{
	if(stopNow(Moves, Removable, TilesMap))
		return false;
	std::vector<std::pair<int, int>> newMoves;

	std::vector<std::tuple<double, double, double, int, int>> LogicalBoardBack;
	std::vector<int> RemovableFalseBack;
	std::vector<int> RemovableTrueBack;
	std::map<int, int> TilesMapBack;
	std::vector<int> WhatsLeftBack;
	std::map<std::tuple<double, double, double>, int> mOccupationBoardBack;
	bool full = false;
	if (Moves.size() >= (_index + 6) && Move.first == Moves[_index + 1].first && Move.first == Moves[_index + 2].first)
	{
		full = true;
		Solution.emplace_back(std::pair(Move.first, Move.second));
		Solution.emplace_back(std::pair(Moves[_index + 5].first, Moves[_index + 5].second));

		RemoveTile(Move.first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		RemoveTile(Move.second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		RemoveTile(Moves[_index + 5].first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		RemoveTile(Moves[_index + 5].second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		/*for (auto& item : Moves)
		{
			if (Move.first != item.first && Move.second != item.first &&
				Move.first != item.second && Move.second != item.second &&
				Moves[_index + 5].first != item.first && Moves[_index + 5].second != item.first &&
				Moves[_index + 5].first != item.second && Moves[_index + 5].second != item.second)
				newMoves.emplace_back(item);
		}*/
		SetMoves(LogicalBoard, Removable, newMoves);
	}
	else
	{
		RemoveTile(Move.first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		RemoveTile(Move.second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableFalseBack, RemovableTrueBack, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
		/*for (auto& item : Moves)
		{
			if (Move.first != item.first && Move.second != item.second)
				newMoves.emplace_back(item);
		}*/
		SetMoves(LogicalBoard, Removable, newMoves);
		Solution.emplace_back(Move);
	}

	if (LogicalBoard.empty())
		return true;

	int index = 0;
	for (auto& move : newMoves)
	{
		auto ret = SolveRec(move, index, newMoves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution);
		++index;
		if (ret)
		{
			return true;
		}
	}

	if(full)
		Solution.pop_back();
	Solution.pop_back();
	/**/
	for (auto& item : LogicalBoardBack) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableFalseBack) Removable[item] = true;
	for (auto& item : RemovableTrueBack) Removable[item] = false;
	for (auto& item : TilesMapBack) TilesMap[item.first] = item.second;
	for (auto& item : WhatsLeftBack) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardBack) mOccupationBoard[item.first] = item.second;
	/**/

	return false;
}

// Just to work on a copy.
inline bool SolveRecInit(
	std::vector<std::pair<int, int>> Moves,
	std::vector<std::tuple<double, double, double, int, int>> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<std::tuple<double, double, double>, int> mOccupationBoard, std::vector<std::pair<int, int>>& Solution)
{
	Solution.clear();
	//hashtable.clear();
	int index = 0;
	for (auto& move : Moves)
	{
		auto ret = SolveRec(move, index, Moves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution);
		++index;
		if (ret)
		{
			return true;
		}
	}

	return false;
}
