#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>
#include <sstream>

class Domino
{
private:
	Domino() : Rank(-1), Pairing(-1) {}
public:

	const int Rank;
	const int Pairing;
	Domino(const int _domino) : Rank(_domino), Pairing(34 <= _domino && _domino < 38 ? 34 : 38 <= _domino && _domino < 42 ? 38 : _domino) {}

	// Copy constructor
	Domino(const Domino& _domino) : Rank(_domino.Rank), Pairing(_domino.Pairing) {}

	// Move constructor
	Domino(Domino&& _domino) noexcept : Rank(_domino.Rank), Pairing(_domino.Pairing) {}

	// Assignment operator
	// Damn sort on LogicalBoard was messing with the values...
	// https://stackoverflow.com/questions/4136156/const-member-and-assignment-operator-how-to-avoid-the-undefined-behavior/63489092#63489092
	Domino& operator=(Domino&& other) noexcept
	{
		*const_cast<int*> (&Rank) = other.Rank;
		*const_cast<int*> (&Pairing) = other.Pairing;
		return *this;
	}

	//Domino(const int _domino, const int _appairage) : Rank(_domino), Pairing(_appairage) {}

	bool operator==(const Domino& other) const
	{
		return Rank == other.Rank && Pairing == other.Pairing;
	}

	Domino operator=(const Domino& other) const
	{
		return Domino(other);
	}

	bool operator<(const Domino& other) const
	{
		return Rank < other.Rank || Rank == other.Rank && Pairing < other.Pairing;
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
	//DominoIndex(DominoIndex&& dominoindex) noexcept : domino(Domino(dominoindex.domino)), index(dominoindex.index) {}
	// Assignment operator
	/*DominoIndex& operator=(DominoIndex&& other) noexcept
	{
		*const_cast<int*> (&Domino) = other.Domino;
		*const_cast<int*> (&index) = other.index;
		return *this;
	}*/
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

constexpr std::array<Coordinates, 144> InitIndexToCoord(const std::array<std::array<std::array<bool, 4>, 8>, 12> & BasePattern)
{
	std::array<Coordinates, 144> InitIndexToCoord;
	int index = 0;
	for (int z = 0; z < 4; ++z)
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

constexpr std::array<std::array<std::array<bool, 4>, 8>, 12> InitBasePattern()
{
	std::array<std::array<std::array<bool, 4>, 8>, 12> arrBasePattern;
	for (int z = 0; z < 4; ++z)
		for (int y = 0; y < 8; ++y)
			for (int x = 0; x < 12; ++x)
				arrBasePattern[x][y][z] = false;
	for (int x = 0; x < 12; ++x)
	{
		arrBasePattern[x][0][0] = true;
		arrBasePattern[x][7][0] = true;
		arrBasePattern[x][3][0] = true;
		arrBasePattern[x][4][0] = true;
	}

	for (int x = 2; x < 10; ++x)
	{
		arrBasePattern[x][1][0] = true;
		arrBasePattern[x][6][0] = true;
	}
	for (int x = 1; x < 11; ++x)
	{
		arrBasePattern[x][2][0] = true;
		arrBasePattern[x][5][0] = true;
	}
	for (int x = 3; x < 9; ++x)
		for (int y = 1; y < 7; ++y)
			arrBasePattern[x][y][1] = true;

	for (int x = 4; x < 8; ++x)
		for (int y = 2; y < 6; ++y)
			arrBasePattern[x][y][2] = true;

	for (int x = 5; x < 7; ++x)
		for (int y = 3; y < 5; ++y)
			arrBasePattern[x][y][3] = true;

	return arrBasePattern;
}

constexpr std::array<bool, 144> InitRemovable()
{
	std::array<bool, 144> arrRemovable;

	for (int i = 0; i < 144; ++i)
		arrRemovable[i] = false;

	arrRemovable[0x0] = true;
	arrRemovable[0xb] = true;
	arrRemovable[0xc] = true;
	arrRemovable[0x13] = true;
	arrRemovable[0x14] = true;
	arrRemovable[0x1d] = true;
	arrRemovable[0x36] = true;
	arrRemovable[0x3f] = true;
	arrRemovable[0x40] = true;
	arrRemovable[0x47] = true;
	arrRemovable[0x48] = true;
	arrRemovable[0x53] = true;
	arrRemovable[0x54] = true;
	arrRemovable[0x59] = true;
	arrRemovable[0x5a] = true;
	arrRemovable[0x5f] = true;
	arrRemovable[0x60] = true;
	arrRemovable[0x65] = true;
	arrRemovable[0x66] = true;
	arrRemovable[0x6b] = true;
	arrRemovable[0x6c] = true;
	arrRemovable[0x71] = true;
	arrRemovable[0x72] = true;
	arrRemovable[0x77] = true;
	arrRemovable[0x78] = true;
	arrRemovable[0x7b] = true;
	arrRemovable[0x7c] = true;
	arrRemovable[0x7f] = true;
	arrRemovable[0x80] = true;
	arrRemovable[0x83] = true;
	arrRemovable[0x84] = true;
	arrRemovable[0x87] = true;
	arrRemovable[0x8c] = true;
	arrRemovable[0x8e] = true;
	arrRemovable[0x8f] = true;

	return arrRemovable;
}

// Available initial positions in the turtles.
constexpr std::array<std::array<std::array<bool, 4>, 8>, 12> arrBasePattern = InitBasePattern();
// Index to coordinates (x, y, z as Double) of available initial positions in the turtles.
constexpr std::array<Coordinates, 144> arrIndexToCoord = InitIndexToCoord(arrBasePattern);

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
	const Domino getDominoFromIndex(const int index) { return mIndexToTile.find(index)->second; }
	const bool getRemovableFromIndex(const int index) { return arrRemovable[index]; }
	bool RemovePairOfTiles(const int, const int);
	const bool IsLocked() { return Moves.size() == 0; }
	const int HowManyMovesLeft() { return Moves.size(); }
	const std::vector<std::pair<int, int>>& GetMovesLeft() { return Moves; }
	bool IsEmpty() { return WhatsLeft.empty(); }
	int getNumberOfTilesLeft() { return WhatsLeft.size(); }
	const std::vector<int>& getWhatsLeft() { return WhatsLeft; }
	const std::vector<DominoIndex>& getLogicalBoard() { return LogicalBoard; }
	void SortBoard(const uint8_t direction);
	bool Solve();
#ifdef _DEBUG
	bool Test();
	bool TestLocked();
#endif
	const std::vector<std::pair<int, int>>& GetSolution() { return Solution; }
	const std::vector<std::pair<int, int>>& GetHistory() { return History; }

	// For the brute force.
	const std::map<int, Domino> & getTilesMap() { return mIndexToTile; }
	const std::array<bool, 144> & getRemovable() { return arrRemovable; }
	const std::map<Coordinates, int> & getOccupationBoard() { return mOccupationBoard; }

private:
	std::vector<std::pair<int, int>> Solution;
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft; // Index
	std::map<int, Domino> mIndexToTile; // index -> domino
	std::map<Coordinates, int> mOccupationBoard; // (x, y, z) -> index
	std::vector<DominoIndex> LogicalBoard; // (domino, index)
	std::array<bool, 144> arrRemovable = InitRemovable();
	void RemoveTile(int);
	void BuildMoves(std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& Moves);
	std::vector<std::pair<int, int>> Moves;
	void SetMoves();
};
