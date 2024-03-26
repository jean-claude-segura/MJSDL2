#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <random>

class Board
{
public:
	Board();
	void InitBoard();
	const std::array<std::tuple<int, int, int, int>, 140>& getBoard();
	const std::array <int, 4>& getSpeciaux();
	const std::array<bool, 144>& getRemovable();
	void Remove(int, int);
	
private:
	std::array<std::tuple<int, int, int, int>, 140> LogicalBoard;
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
};
