#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <random>

class Board
{
public:
	Board();
	void FillBoard();
	int dominos[42] = {
		4,4,4,4,4,4,4,4,4, // Cercles
		4,4,4,4,4,4,4,4,4, // Bambous
		4,4,4,4,4,4,4,4,4, // Caractères
		1,1,1,1, // Saisons
		1,1,1,1, // Fleurs
		4,4,4,4, // Honeurs
		4,4,4 // Dragons
	};
	const std::array<std::tuple<int, int, int, int>, 140>& getBoard();
	const std::array <int, 4>& getSpeciaux();
	
private:
	std::array<std::tuple<int, int, int, int>, 140> LogicalBoard;
	std::array <int, 4> Speciaux;
	std::array<std::array<std::array<int, 5>, 8>, 12> BasePattern;
};
