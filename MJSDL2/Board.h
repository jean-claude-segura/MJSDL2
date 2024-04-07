#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>

class Board
{
private:
	static bool CompLogicalBoardDownLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardUpLeft(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardUpRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompLogicalBoardDownRight(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);
	static bool CompRemovableBoard(const std::tuple<double, double, double, int, int>& left, const std::tuple<double, double, double, int, int>& right);

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
	const std::vector<std::tuple<double, double, double, int, int>>& getLogicalBoard() { return LogicalBoard; }
	void SortBoard(const uint8_t direction);
	
private:
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
