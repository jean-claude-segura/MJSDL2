#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>

constexpr std::array<std::tuple<double, double, double>, 144> InitIndexToCoord()
{
	std::array<std::tuple<double, double, double>, 144> InitIndexToCoord;
	std::array<std::array<std::array<bool, 5>, 8>, 12> BasePattern;
	for (int z = 0; z < 5; ++z)
		for (int y = 0; y < 8; ++y)
			for (int x = 0; x < 12; ++x)
				BasePattern[x][y][z] = false;
	for (int x = 0; x < 12; ++x)
	{
		BasePattern[x][0][0] = true;
		BasePattern[x][7][0] = true;
		BasePattern[x][3][0] = true;
		BasePattern[x][4][0] = true;
	}

	for (int x = 2; x < 10; ++x)
	{
		BasePattern[x][1][0] = true;
		BasePattern[x][6][0] = true;
	}
	for (int x = 1; x < 11; ++x)
	{
		BasePattern[x][2][0] = true;
		BasePattern[x][5][0] = true;
	}
	for (int x = 3; x < 9; ++x)
		for (int y = 1; y < 7; ++y)
			BasePattern[x][y][1] = true;

	for (int x = 4; x < 8; ++x)
		for (int y = 2; y < 6; ++y)
			BasePattern[x][y][2] = true;

	for (int x = 5; x < 7; ++x)
		for (int y = 3; y < 5; ++y)
			BasePattern[x][y][3] = true;

	int index = 0;
	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) InitIndexToCoord[index++] = std::make_tuple(x, y, z);
			}
		}
	}

	InitIndexToCoord[140] = std::make_tuple(-1, 3.5, 0);
	InitIndexToCoord[141] = std::make_tuple(12, 3.5, 0);
	InitIndexToCoord[142] = std::make_tuple(13, 3.5, 0);
	InitIndexToCoord[143] = std::make_tuple(5.5, 3.5, 4);

	return InitIndexToCoord;
}

static constexpr std::array<std::tuple<double, double, double>, 144> IndexToCoord = InitIndexToCoord();

class Board
{
private:
	static bool CompLogicalBoardDownLeft(const std::pair<int, int>& left, const std::pair<int, int>& right);
	static bool CompLogicalBoardUpLeft(const std::pair<int, int>& left, const std::pair<int, int>& right);
	static bool CompLogicalBoardUpRight(const std::pair<int, int>& left, const std::pair<int, int>& right);
	static bool CompLogicalBoardDownRight(const std::pair<int, int>& left, const std::pair<int, int>& right);
	static bool CompRemovableBoard(const std::pair<int, int>& left, const std::pair<int, int>& right);

public:
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
	const std::vector<std::pair<int, int>>& getLogicalBoard() { return LogicalBoard; }
	void SortBoard(const uint8_t direction);
	bool Solve();
	const std::vector<std::pair<int, int>>& GetSolution() { return Solution; }

private:
	std::vector<std::pair<int, int>> Solution;
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft; // Index
	std::map<int, int> TilesMap; // index -> domino
	std::map<std::tuple<double, double, double>, int> mOccupationBoard; // (x, y, z) -> index
	std::vector<std::pair<int, int>> LogicalBoard; // (x, y, z, domino, index)
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
	void BuildMoves(std::vector<std::pair<int, int>>& RemovableBoard, std::vector<std::pair<int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves);
	std::vector<std::pair<int, int>> Moves;
	void SetMoves();
};

inline void RemoveTile(
	const int index,
	std::vector<std::pair<int, int>>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<std::tuple<double, double, double>, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<std::pair<int, int>>& LogicalBoardBack,
	std::vector<int>& RemovableWasTrue,
	std::vector<int>& RemovableWasFalse,
	std::map<int, int>& TilesMapBack,
	std::vector<int>& WhatsLeftBack,
	std::map<std::tuple<double, double, double>, int>& mOccupationBoardBack
	)
{
	TilesMapBack[index] = TilesMap[index];
	TilesMap.erase(index);

	std::vector<std::pair<int, int>>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && (*it).second != index; ++it);
	auto coord = IndexToCoord[it->second];
	double x = std::get<0>(coord);
	double y = std::get<1>(coord);
	double z = std::get<2>(coord);

	LogicalBoardBack.emplace_back(*it);
	LogicalBoard.erase(it);
	mOccupationBoardBack[coord] = index;
	mOccupationBoard.erase(coord);

	auto itWL = std::find(WhatsLeft.begin(), WhatsLeft.end(), index);
	if (itWL != WhatsLeft.end())
	{
		WhatsLeftBack.emplace_back(*itWL);
		WhatsLeft.erase(itWL);
	}

	if(Removable[index]) RemovableWasTrue.emplace_back(index);
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
		if (x < 11 && mOccupationBoard.contains(std::make_tuple(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x + 1, y, z + 1))))
		{
			auto index = mOccupationBoard[std::make_tuple(x + 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (x > 0 && mOccupationBoard.contains(std::make_tuple(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[std::make_tuple(x - 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (z > 0) // mOccupationBoard[std::make_tuple(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(std::make_tuple(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[std::make_tuple(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
			if (x > 0 && !mOccupationBoard.contains(std::make_tuple(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[std::make_tuple(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
		}
	}
}

inline void BuildMoves(const std::vector<std::pair<int, int>>& RemovableBoard, std::vector<std::pair<int, int>>::const_iterator& itFirst, std::vector<std::vector<int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).first;
		auto itNext = itFirst;
		std::vector<int> temp;
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
				if (temp.empty())
					temp.emplace_back(itFirst->second);
				temp.emplace_back(itNext->second);
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

inline void SetMoves(const std::vector<std::pair<int, int>>& LogicalBoard, const std::array<bool, 144>& Removable, std::vector<std::vector<int>>& Moves)
{
	std::vector<std::pair<int, int>> RemovableBoard; // (x, y, z, domino, index)
	for (const auto& tuple : LogicalBoard)
	{
		if (Removable[tuple.second]) RemovableBoard.emplace_back(tuple);
	}

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const std::pair<int, int>& left, const std::pair<int, int>& right)
		{
			return left.first < right.first;
		});
	auto itFirst = RemovableBoard.begin();
	Moves.clear();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

static std::map<uint64_t, std::array<uint64_t, 144 / 8>> hashtable;
static std::vector<std::array<uint64_t, 144 / 8>> vBoardDescription;

// Pseudo-hash.
// Collision rate to check.
inline uint64_t getHash(const std::vector<std::vector<int>>& Moves,
	const std::array<bool, 144>& Removable,
	const std::map<int, int>& TilesMap
	)
{
	// 64 56 48 40 32 24 16 8
	uint64_t indexsum = 0ULL; // 14 : 14 + 8 + 6 + 8 + 13
	uint64_t tilesum = 0ULL; // 14 : 8 + 6 + 8 + 13
	uint64_t removables = 0ULL; // 8 : 6 + 8 + 13
	uint64_t nmoves = 0ULL; // 6 (max 40 moves) : 8 + 13
	uint64_t count = 0ULL; // 8 : 14

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
	auto rTile = alternatetilesum % 10; // 7
	rIndex = rIndex == 0 ? 0 : 10 - rIndex;
	rTile = rTile == 0 ? 0 : 10 - rTile;

	uint64_t hash = indexsum << (14 + 8 + 6 + 8 + 14) | tilesum << (8 + 6 + 8 + 14) | removables << (6 + 8 + 14) | nmoves << (8 + 14) | count << (14) | rIndex << 7 | rTile;
	
	return hash;
}

inline bool stopNow(std::vector<std::vector<int>>& Moves,
	const std::array<bool, 144>& Removable,
	const std::map<int, int>& TilesMap
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

	if (hashtable.contains(hash))
	{
		auto boardDescriptionInHashtable = hashtable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
#ifdef _DEBUG
				std::cout << "Collision de hash" << std::endl;
#endif
				if (vBoardDescription.end() == std::find(vBoardDescription.begin(), vBoardDescription.end(), boardDescription))
				{
					vBoardDescription.emplace_back(boardDescription);
					return false;
				}
				else
				{
					return true;
				}
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
	const std::vector<int>& Move,
	int _index,
	const std::vector<std::vector<int>> & Moves,
	std::vector<std::pair<int, int>> & LogicalBoard,
	std::array<bool, 144> & Removable,
	std::map<int, int> & TilesMap,
	std::vector<int> & WhatsLeft,
	std::map<std::tuple<double, double, double>, int> & mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{

	std::vector<std::vector<int>> newMoves;

	std::vector<std::pair<int, int>> LogicalBoardBack;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, int> TilesMapBack;
	std::vector<int> WhatsLeftBack;
	std::map<std::tuple<double, double, double>, int> mOccupationBoardBack;
	bool full = false;
	for (const auto & move : Move)
	{
		RemoveTile(move,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard, LogicalBoardBack, RemovableWasTrue, RemovableWasFalse, TilesMapBack, WhatsLeftBack, mOccupationBoardBack);
	}
	Solution.emplace_back(std::make_pair(Move[0], Move[1]));
	if (Move.size() == 4)
	{
		full = true;
		Solution.emplace_back(std::make_pair(Move[2], Move[3]));
	}

	SetMoves(LogicalBoard, Removable, newMoves);

	if (LogicalBoard.empty())
	{
		/**/
		for (auto& item : LogicalBoardBack) LogicalBoard.emplace_back(item);
		for (auto& item : RemovableWasTrue) Removable[item] = true;
		for (auto& item : RemovableWasFalse) Removable[item] = false;
		for (auto& item : TilesMapBack) TilesMap[item.first] = item.second;
		for (auto& item : WhatsLeftBack) WhatsLeft.emplace_back(item);
		for (auto& item : mOccupationBoardBack) mOccupationBoard[item.first] = item.second;
		/**/

		return true;
	}

	if (!stopNow(newMoves, Removable, TilesMap))
	{
		int index = 0;
		for (auto& move : newMoves)
		{
			auto ret = SolveRec(move, index, newMoves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution);
			++index;
			if (ret)
			{
				/**/
				for (auto& item : LogicalBoardBack) LogicalBoard.emplace_back(item);
				for (auto& item : RemovableWasTrue) Removable[item] = true;
				for (auto& item : RemovableWasFalse) Removable[item] = false;
				for (auto& item : TilesMapBack) TilesMap[item.first] = item.second;
				for (auto& item : WhatsLeftBack) WhatsLeft.emplace_back(item);
				for (auto& item : mOccupationBoardBack) mOccupationBoard[item.first] = item.second;
				/**/

				return true;
			}
		}
	}
	if(full)
		Solution.pop_back();
	Solution.pop_back();
	/**/
	for (auto& item : LogicalBoardBack) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapBack) TilesMap[item.first] = item.second;
	for (auto& item : WhatsLeftBack) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardBack) mOccupationBoard[item.first] = item.second;
	/**/

	return false;
}

// Just to work on a copy.
inline bool SolveRecInit(
	std::vector<std::pair<int, int>> oldMoves,
	std::vector<std::pair<int, int>> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<std::tuple<double, double, double>, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{
#ifdef _DEBUG
	std::vector<std::pair<int, int>> LogicalBoardBack = LogicalBoard;
	std::array<bool, 144> RemovableBack = Removable;
	std::map<int, int> TilesMapBack = TilesMap;
	std::vector<int> WhatsLeftBack = WhatsLeft;
	std::map<std::tuple<double, double, double>, int> mOccupationBoardBack = mOccupationBoard;
#endif

	Solution.clear();
	// Must be cleared. The hash just means the positions has been seen at least once before. It doesn't mean it was in a lost game.
	// And the start position is always different.
	hashtable.clear();
	bool ret = false;
	int index = 0;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	for(int _index = 0; _index < oldMoves.size();)
	{
		std::vector<int> temp;
		temp.emplace_back(oldMoves[_index].first);
		temp.emplace_back(oldMoves[_index].second);
		if (oldMoves.size() >= (_index + 6) && oldMoves[_index].first == oldMoves[_index + 1].first && oldMoves[_index].first == oldMoves[_index + 2].first)
		{
			temp.emplace_back(oldMoves[_index + 5].first);
			temp.emplace_back(oldMoves[_index + 5].second);
			_index += 6;
		}
		else
		{
			++_index;
		}
		Moves.emplace_back(temp);
	}
	for (auto& move : Moves)
	{
		ret = SolveRec(move, index, Moves, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution);
		++index;
		if (ret) break;
	}

#ifdef _DEBUG
	bool LogicalBoardOk = LogicalBoardBack.size() == LogicalBoard.size();
	bool RemovableOk = RemovableBack.size() == Removable.size();
	bool TilesMapOk = TilesMapBack.size() == TilesMap.size();
	bool WhatsLeftOk = WhatsLeftBack.size() == WhatsLeft.size();
	bool mOccupationBoardOk = mOccupationBoardBack.size() == mOccupationBoard.size();
	for (auto& item : LogicalBoardBack) LogicalBoardOk &= LogicalBoard.end() != std::find(LogicalBoard.begin(), LogicalBoard.end(), item);
	for (int i = 0;  i < RemovableBack.size(); ++i) RemovableOk &= Removable[i] == RemovableBack[i];
	for (auto& item : TilesMapBack) TilesMapOk &= TilesMap.contains(item.first) && TilesMap[item.first] == item.second;
	for (auto& item : WhatsLeftBack) WhatsLeftOk &= WhatsLeftBack.end() != std::find(WhatsLeftBack.begin(), WhatsLeftBack.end(), item);
	for (auto& item : mOccupationBoardBack)  mOccupationBoardOk &= mOccupationBoard.contains(item.first) && mOccupationBoard[item.first] == item.second;

	std::cout << LogicalBoardOk << std::endl;
	std::cout << RemovableOk << std::endl;
	std::cout << TilesMapOk << std::endl;
	std::cout << WhatsLeftOk << std::endl;
	std::cout << mOccupationBoardOk << std::endl;
#endif

	return ret;
}
