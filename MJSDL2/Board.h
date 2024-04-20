#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>

class Domino
{
private:
	Domino() : rang(-1), appairage(-1) {}
public:

	const int rang;
	const int appairage;
	Domino(const int _domino) : rang(_domino), appairage(34 <= _domino && _domino < 38 ? 34 : 38 <= _domino && _domino < 42 ? 38 : _domino) {}

	// Copy constructor
	Domino(const Domino& _domino) : rang(_domino.rang), appairage(_domino.appairage) {}

	// Move constructor
	Domino(Domino&& _domino) : rang(_domino.rang), appairage(_domino.appairage) {}

	// Assignment operator
	// Damn sort on LogicalBoard was messing with the values...
	// https://stackoverflow.com/questions/4136156/const-member-and-assignment-operator-how-to-avoid-the-undefined-behavior/63489092#63489092
	Domino& operator=(Domino&& other)
	{
		*const_cast<int*> (&rang) = other.rang;
		*const_cast<int*> (&appairage) = other.appairage;
		return *this;
	}

	//Domino(const int _domino, const int _appairage) : rang(_domino), appairage(_appairage) {}

	bool operator==(const Domino& other) const
	{
		return rang == other.rang && appairage == other.appairage;
	}

	Domino operator=(const Domino& other) const
	{
		return Domino(other);
	}

	bool operator<(const Domino& other) const
	{
		return rang < other.rang || rang == other.rang && appairage < other.appairage;
	}
}; 

class DominoIndex
{
private:
	DominoIndex() : domino(Domino(-1)), index(-1) {}
public:
	Domino domino;
	int index;
	DominoIndex(const Domino & _domino, int _index) : domino(Domino(_domino)), index(_index) {}

	bool operator==(const DominoIndex& other) const
	{
		return domino == other.domino && index == other.index;
	}

	// copy constructor
	//DominoIndex(const DominoIndex& dominoindex) : domino(Domino(dominoindex.domino)), index(dominoindex.index) {}
	// Move constructor
	//DominoIndex(DominoIndex&& dominoindex) : domino(Domino(dominoindex.domino)), index(dominoindex.index) {}
};

// https://www.ibm.com/docs/en/i/7.5?topic=only-constexpr-constructors-c11
class Coordinates
{
public:
	double x;
	double y;
	double z;
	constexpr Coordinates() { x = 0.; y = 0.; z = 0.; };
	constexpr Coordinates(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

	bool operator==(const Coordinates& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator<(const Coordinates& other) const
	{
		return z < other.z ||
			(z == other.z && y < other.y) ||
			(z == other.z && y == other.y && x < other.x);
	}
};

constexpr std::array<Coordinates, 144> InitIndexToCoord(const std::array<std::array<std::array<bool, 5>, 8>, 12> & BasePattern)
{
	std::array<Coordinates, 144> InitIndexToCoord;
	int index = 0;
	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) {
					InitIndexToCoord[index++] = { double(x), double(y), double(z) };
				}
			}
		}
	}

	InitIndexToCoord[index++] = { -1., 3.5, 0. };
	InitIndexToCoord[index++] = { 12., 3.5, 0. };
	InitIndexToCoord[index++] = { 13., 3.5, 0. };
	InitIndexToCoord[index++] = { 5.5, 3.5, 4. };

	return InitIndexToCoord;
}

constexpr std::array<std::array<std::array<int, 5>, 8>, 12> InitBaseTurtlePattern(const std::array<std::array<std::array<bool, 5>, 8>, 12>& BasePattern)
{
	std::array<std::array<std::array<int, 5>, 8>, 12> BaseTurtlePattern;
	int index = 0;
	for (int z = 0; z < 5; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (BasePattern[x][y][z]) {
					BaseTurtlePattern[x][y][z] = index;
				}
				else
				{
					BaseTurtlePattern[x][y][z] = -1;
				}
			}
		}
	}

	return BaseTurtlePattern;
}

constexpr std::array<std::array<std::array<bool, 5>, 8>, 12> InitBasePattern()
{
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

	return BasePattern;
}

static constexpr std::array<std::array<std::array<bool, 5>, 8>, 12> BasePattern = InitBasePattern();
static constexpr std::array<Coordinates, 144> IndexToCoord = InitIndexToCoord(BasePattern);
static constexpr std::array<std::array<std::array<int, 5>, 8>, 12> BaseTurtlePattern = InitBaseTurtlePattern(BasePattern);

class Board
{
private:
	static bool CompLogicalBoardDownLeft(const DominoIndex& left, const DominoIndex& right);
	static bool CompLogicalBoardUpLeft(const DominoIndex& left, const DominoIndex& right);
	static bool CompLogicalBoardUpRight(const DominoIndex& left, const DominoIndex& right);
	static bool CompLogicalBoardDownRight(const DominoIndex& left, const DominoIndex& right);
	static bool CompRemovableBoard(const DominoIndex& left, const DominoIndex& right);

public:
	Board();
	void InitBoard();
	const Domino getDominoFromIndex(const int index) { return TilesMap.find(index)->second; }
	const bool getRemovableFromIndex(const int index) { return Removable[index]; }
	bool RemovePairOfTiles(const int, const int);
	const bool IsBlocked() { return Moves.size() == 0; }
	const int HowManyMovesLeft() { return Moves.size(); }
	const std::vector<std::pair<int, int>>& GetMovesLeft() { return Moves; }
	bool IsEmpty() { return WhatsLeft.empty(); }
	int getNumberOfTilesLeft() { return WhatsLeft.size(); }
	const std::vector<int>& getWhatsLeft() { return WhatsLeft; }
	const std::vector<DominoIndex>& getLogicalBoard() { return LogicalBoard; }
	void SortBoard(const uint8_t direction);
	bool Solve();
	const std::vector<std::pair<int, int>>& GetSolution() { return Solution; }
	const std::vector<std::pair<int, int>>& GetHistory() { return History; }

	// For the brute force.
	const std::map<int, Domino> & getTilesMap() { return TilesMap; }
	const std::array<bool, 144> & getRemovable() { return Removable; }
	const std::map<Coordinates, int> & getOccupationBoard() { return mOccupationBoard; }

private:
	std::vector<std::pair<int, int>> Solution;
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft; // Index
	std::map<int, Domino> TilesMap; // index -> domino
	std::map<Coordinates, int> mOccupationBoard; // (x, y, z) -> index
	std::vector<DominoIndex> LogicalBoard; // (domino, index)
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
	void BuildMoves(std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves);
	std::vector<std::pair<int, int>> Moves;
	void SetMoves();
};
