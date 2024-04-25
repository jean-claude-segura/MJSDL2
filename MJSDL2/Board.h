#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>
#include <sstream>

class Tile
{
private:
	//Tile() : Rank(-1), Pairing(-1) {}
public:
	Tile() : Rank(-1), Pairing(-1) {}
	const int Rank;
	const int Pairing;
	Tile(const int rank) : Rank(rank), Pairing(34 <= rank && rank < 38 ? 34 : 38 <= rank && rank < 42 ? 38 : rank) {}

	// Copy constructor
	Tile(const Tile& tile) : Rank(tile.Rank), Pairing(tile.Pairing) {}

	// Move constructor
	Tile(Tile&& tile) noexcept : Rank(tile.Rank), Pairing(tile.Pairing) {}

	// Assignment operator
	// Damn sort on vLogicalBoard was messing with the values...
	// https://stackoverflow.com/questions/4136156/const-member-and-assignment-operator-how-to-avoid-the-undefined-behavior/63489092#63489092
	Tile& operator=(Tile&& other) noexcept
	{
		*const_cast<int*> (&Rank) = other.Rank;
		*const_cast<int*> (&Pairing) = other.Pairing;
		return *this;
	}

	//Tile(const int rank, const int _appairage) : Rank(rank), Pairing(_appairage) {}

	bool operator==(const Tile& other) const
	{
		return Rank == other.Rank && Pairing == other.Pairing;
	}

	Tile operator=(const Tile& other) const
	{
		return Tile(other);
	}

	bool operator<(const Tile& other) const
	{
		return Rank < other.Rank || Rank == other.Rank && Pairing < other.Pairing;
	}
}; 

class TileAndIndex
{
private:
	TileAndIndex() : TileObject(Tile(-1)), Index(-1) {}
public:
	Tile TileObject;
	int Index;
	int X; 
	int Y;
	int Z;
	int DecX;
	int DecY;
	TileAndIndex(const Tile & tile, int index, int x, int y, int z, int decX, int decY) : TileObject(Tile(tile)), Index(index), X(x), Y(y), Z(z), DecX(decX), DecY(decY) {}

	bool operator==(const TileAndIndex& other) const
	{
		return TileObject == other.TileObject && Index == other.Index && X == other.X && Y == other.Y && Z == other.Z && DecX == other.DecX && DecY == other.DecY;
	}

	// copy constructor
	//TileAndIndex(const TileAndIndex& dominoindex) : TileObject(Tile(dominoindex.TileObject)), Index(dominoindex.Index) {}
	// Move constructor
	//TileAndIndex(TileAndIndex&& dominoindex) noexcept : TileObject(Tile(dominoindex.TileObject)), Index(dominoindex.Index) {}
	// Assignment operator
	/*TileAndIndex& operator=(TileAndIndex&& other) noexcept
	{
		*const_cast<int*> (&Tile) = other.Tile;
		*const_cast<int*> (&Index) = other.Index;
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

constexpr std::array<Coordinates, 144> InitIndexToCoord(const std::array<std::array<std::array<bool, 4>, 8>, 12> & arrBasePattern)
{
	std::array<Coordinates, 144> arrInitIndexToCoord;
	int index = 0;
	for (int z = 0; z < 4; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (arrBasePattern[x][y][z]) {
					arrInitIndexToCoord[index++] = { double(x), double(y), double(z) };
				}
			}
		}
	}

	arrInitIndexToCoord[index++] = { -1., 3.5, 0. };
	arrInitIndexToCoord[index++] = { 12., 3.5, 0. };
	arrInitIndexToCoord[index++] = { 13., 3.5, 0. };
	arrInitIndexToCoord[index++] = { 5.5, 3.5, 4. };

	return arrInitIndexToCoord;
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

constexpr std::array<std::tuple<int, int, int, int, int>, 144> InitIndexToBoardCoord(const std::array<std::array<std::array<bool, 4>, 8>, 12>& arrBasePattern)
{
	std::array<std::tuple<int, int, int, int, int>, 144> arrBaseTurtlePatternToCoord;
	int index = 0;
	for (int z = 0; z < 4; ++z)
	{
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 12; ++x)
			{
				if (arrBasePattern[x][y][z]) {
					arrBaseTurtlePatternToCoord[index++] = { x, y, z, 0, 0 };
				}
			}
		}
	}

	arrBaseTurtlePatternToCoord[index++] = { -1, 3, 0, 0, 1 };
	arrBaseTurtlePatternToCoord[index++] = { 12, 3, 0, 0, 1 };
	arrBaseTurtlePatternToCoord[index++] = { 13, 3, 0, 0, 1 };
	arrBaseTurtlePatternToCoord[index++] = { 5, 3, 4, 1, 1 };

	return arrBaseTurtlePatternToCoord;
}

// Available initial positions in the turtles.
constexpr std::array<std::array<std::array<bool, 4>, 8>, 12> arrBasePattern = InitBasePattern();
// Index to coordinates (x, y, z as Double) of available initial positions in the turtles.
constexpr std::array<Coordinates, 144> arrIndexToCoord = InitIndexToCoord(arrBasePattern);
// Gets position from the Index
constexpr std::array<std::tuple<int, int, int, int, int>, 144> arrIndexToBoardCoord = InitIndexToBoardCoord(arrBasePattern);

class Board
{
private:
	static bool CompLogicalBoardDownLeft(const TileAndIndex& left, const TileAndIndex& right);
	static bool CompLogicalBoardUpLeft(const TileAndIndex& left, const TileAndIndex& right);
	static bool CompLogicalBoardUpRight(const TileAndIndex& left, const TileAndIndex& right);
	static bool CompLogicalBoardDownRight(const TileAndIndex& left, const TileAndIndex& right);
	static bool CompRemovableBoard(const TileAndIndex& left, const TileAndIndex& right);

public:
	Board();
	void InitBoard();
	const Tile getDominoFromIndex(const int index) { return mIndexToTile.find(index)->second; }
	const bool getRemovableFromIndex(const int index) { return arrRemovable[index]; }
	bool RemovePairOfTiles(const int, const int);
	const bool IsLocked() { return vMoves.size() == 0; }
	const int HowManyMovesLeft() { return vMoves.size(); }
	const std::vector<std::pair<int, int>>& GetMovesLeft() { return vMoves; }
	bool IsEmpty() { return vWhatsLeft.empty(); }
	int getNumberOfTilesLeft() { return vWhatsLeft.size(); }
	const std::vector<int>& getWhatsLeft() { return vWhatsLeft; }
	const std::vector<TileAndIndex>& getLogicalBoard() { return vLogicalBoard; }
	void SortBoard(const uint8_t direction);
	bool Solve();
	bool IsLockedFromStart();
	bool IsLockedFromMove();
#ifdef _DEBUG
	bool Test();
	void InitBoardLockedHorizontal(int test);
	bool TestLocked();
#endif
	const std::vector<std::pair<int, int>>& GetSolution() { return vSolution; }
	const std::vector<std::pair<int, int>>& GetHistory() { return vHistory; }

	// For the brute force.
	const std::map<int, Tile> & getTilesMap() { return mIndexToTile; }
	const std::array<bool, 144> & getRemovable() { return arrRemovable; }
	const std::map<Coordinates, int> & getOccupationBoard() { return mOccupationBoard; }

private:
	std::vector<std::pair<int, int>> vSolution;
	std::vector<std::pair<int, int>> vHistory;
	std::vector<int> vWhatsLeft; // Index
	std::map<int, Tile> mIndexToTile; // Index -> TileObject
	std::map<int, Tile> mIndexToTileRemoved;
	std::map<Coordinates, int> mOccupationBoard; // (x, y, z) -> Index
	std::vector<TileAndIndex> vLogicalBoard; // (TileObject, Index)
	std::array<bool, 144> arrRemovable = InitRemovable();
	void RemoveTile(int);
	void BuildMoves(std::vector<TileAndIndex>& vRemovableBoard, std::vector<TileAndIndex>::iterator& itFirst, std::vector<std::pair<int, int>>& vMoves);
	std::vector<std::pair<int, int>> vMoves;
	void SetMoves();
	bool bIsLockedFromMove;
};
