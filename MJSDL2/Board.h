#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>

class Board
{
public:
	Board();
	void InitBoard();
	const std::array<std::tuple<int, int, int, int>, 140>& getBoard();
	const std::array <int, 4>& getSpeciaux() { return Speciaux; }
	const std::array<bool, 144>& getRemovable() { return removable; }
	void Remove(int, int);
	const bool GetBlocked() { return Blocked; }
	bool IsEmpty() { return Left.empty(); }
	int WhatsLeft() { return Left.size(); }
	const std::vector<int>& getLeft() { return Left; }
	
private:
	std::vector<int> Left;
	std::array<std::tuple<int, int, int, int>, 140> LogicalBoard;
	std::array<std::array<std::array<int, 5>, 8>, 12> OccupationBoard;
	std::array <int, 4> Speciaux;
	std::array<std::array<std::array<int, 5>, 8>, 12> BasePattern;
	std::array<bool, 144> removable = {
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
	void Remove(int);
	bool Blocked = false;
};
