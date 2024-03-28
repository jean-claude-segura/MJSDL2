#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>

class Board
{
public:
	static bool CompLogicalBoard(std::tuple<int, int, int, int, int> left, std::tuple<int, int, int, int, int> right);
	Board();
	void InitBoard();
	const int getDominoFromIndex(int index) { return TilesMap[index]; }
	const std::array <int, 4>& getSpeciaux() { return Speciaux; }
	const bool getRemovableFromIndex(int index) { return Removable[index]; }
	void RemovePairOfTiles(int, int);
	const bool IsBlocked() { return Blocked; }
	bool IsEmpty() { return WhatsLeft.empty(); }
	int getNumberOfTilesLeft() { return WhatsLeft.size(); }
	const std::vector<int>& getWhatsLeft() { return WhatsLeft; }
	const std::vector<std::tuple<int, int, int, int, int>>& getLogicalBoard() { return LogicalBoard; }
	
private:
	std::vector<int> WhatsLeft;
	std::map<int, int> TilesMap;
	std::array<std::tuple<int, int, int>, 140> InitIndexToCoord;
	std::vector<std::tuple<int, int, int, int, int>> LogicalBoard; // x, y, z, domino, index
	std::array<std::array<std::array<int, 5>, 8>, 12> OccupationBoard;
	std::array <int, 4> Speciaux;
	std::array<std::array<std::array<int, 5>, 8>, 12> BasePattern;
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
	bool Blocked = false;
};
