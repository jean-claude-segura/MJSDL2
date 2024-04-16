#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>

void RemoveTile(
	const int index,
	std::vector<std::tuple<double, double, double, int, int>>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<std::tuple<double, double, double>, int>& mOccupationBoard);
void BuildMoves(std::vector<std::tuple<double, double, double, int, int>>& RemovableBoard, std::vector<std::tuple<double, double, double, int, int>>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves);
bool SolveRec(
	std::pair<int, int> Move,
	int _index,
	std::vector<std::pair<int, int>> Moves,
	std::vector<std::tuple<double, double, double, int, int>> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<std::tuple<double, double, double>, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution);
void SetMoves(std::vector<std::tuple<double, double, double, int, int>>& LogicalBoard, std::array<bool, 144>& Removable, std::vector<std::pair<int, int>>& Moves);

static bool CompLogicalBoardDownLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
static bool CompLogicalBoardUpLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
static bool CompLogicalBoardUpRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
static bool CompLogicalBoardDownRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
static bool CompRemovableBoard(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);

class Board
{
private:

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
	void Solve();
	
private:
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft;
	std::map<int, int> TilesMap; // (index, domino)
	std::map<std::tuple<double, double, double>, int> mOccupationBoard;
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
	std::map<std::tuple<double, double, double>, int>& mOccupationBoard)
{
	TilesMap.erase(index);

	std::vector<std::tuple<double, double, double, int, int>>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && std::get<4>(*it) != index; ++it);
	int x = std::get<0>(*it);
	int y = std::get<1>(*it);
	int z = std::get<2>(*it);
	LogicalBoard.erase(it);
	mOccupationBoard.erase(std::make_tuple<double, double, double>(x, y, z));

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
		if (x < 11 && mOccupationBoard.contains(std::make_tuple<double, double, double>(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple<double, double, double>(x + 1, y, z + 1))))
			Removable[mOccupationBoard[std::make_tuple<double, double, double>(x + 1, y, z)]] = true;
		if (x > 0 && mOccupationBoard.contains(std::make_tuple<double, double, double>(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(std::make_tuple<double, double, double>(x - 1, y, z + 1))))
			Removable[mOccupationBoard[std::make_tuple<double, double, double>(x - 1, y, z)]] = true;
		if (z > 0) // mOccupationBoard[std::make_tuple<double, double, double>(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(std::make_tuple<double, double, double>(x + 1, y, z - 1)))
				Removable[mOccupationBoard[std::make_tuple<double, double, double>(x, y, z - 1)]] = true;
			if (x > 0 && !mOccupationBoard.contains(std::make_tuple<double, double, double>(x - 1, y, z - 1)))
				Removable[mOccupationBoard[std::make_tuple<double, double, double>(x, y, z - 1)]] = true;
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

inline bool SolveRec(
	std::pair<int, int> Move,
	int _index,
	std::vector<std::pair<int, int>> Moves,
	std::vector<std::tuple<double, double, double, int, int>> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<std::tuple<double, double, double>, int> mOccupationBoard, std::vector<std::pair<int, int>>& Solution)
{
	if (Moves.size() >= (_index + 6) && Move.first == Moves[_index + 1].first && Move.first == Moves[_index + 2].first)
	{
		Solution.emplace_back(std::pair(Move.first, Move.second));
		Solution.emplace_back(std::pair(Moves[_index + 5].first, Moves[_index + 5].second));

		RemoveTile(Move.first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		RemoveTile(Move.second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		RemoveTile(Moves[_index + 5].first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		RemoveTile(Moves[_index + 5].second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		SetMoves(LogicalBoard, Removable, Moves);
	}
	else
	{
		RemoveTile(Move.first,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		RemoveTile(Move.second,
			LogicalBoard,
			Removable,
			TilesMap,
			WhatsLeft,
			mOccupationBoard);
		SetMoves(LogicalBoard, Removable, Moves);
		Solution.emplace_back(Move);
	}

	if (LogicalBoard.empty())
		return true;

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
	Solution.pop_back();
	return false;
}
