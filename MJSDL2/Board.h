#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <random>
#include <cmath>
#include <map>

class DominoIndex
{
private:
	DominoIndex() {}

public:
	int domino;
	int index;
	DominoIndex(int _domino, int _index) : domino (_domino), index(_index) {}

	bool operator==(const DominoIndex& other) const
	{
		return domino == other.domino && index == other.index;
	}
};

// https://www.ibm.com/docs/en/i/7.5?topic=only-constexpr-constructors-c11
class Coordinates
{
public:
	double x;
	double y;
	double z;
	constexpr Coordinates() { x = 0.; y = 0.; z = 0.; };
	constexpr Coordinates(double _x, double _y, double _z) : x( _x ), y( _y ), z( _z ) {}

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

constexpr std::array<Coordinates, 144> InitIndexToCoord()
{
	std::array<Coordinates, 144> InitIndexToCoord;
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

static constexpr std::array<Coordinates, 144> IndexToCoord = InitIndexToCoord();

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
	const int getDominoFromIndex(const int index) { return TilesMap[index]; }
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

private:
	std::vector<std::pair<int, int>> Solution;
	std::vector<std::pair<int, int>> History;
	std::vector<int> WhatsLeft; // Index
	std::map<int, int> TilesMap; // index -> domino
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

inline void RemoveTile(
	const int index,
	std::vector<DominoIndex>& LogicalBoard,
	std::array<bool, 144>& Removable,
	std::map<int, int>& TilesMap,
	std::vector<int>& WhatsLeft,
	std::map<Coordinates, int>& mOccupationBoard,
	/* *********************************************************************** */
	std::vector<DominoIndex>& LogicalBoardBack,
	std::vector<int>& RemovableWasTrue,
	std::vector<int>& RemovableWasFalse,
	std::map<int, int>& TilesMapBack,
	std::vector<int>& WhatsLeftBack,
	std::map<Coordinates, int>& mOccupationBoardBack
	)
{
	TilesMapBack[index] = TilesMap[index];
	TilesMap.erase(index);

	std::vector<DominoIndex>::iterator it = LogicalBoard.begin();
	for (; it != LogicalBoard.end() && it->index != index; ++it);
	auto coord = IndexToCoord[it->index];
	double x = coord.x;
	double y = coord.y;
	double z = coord.z;

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
		if (x < 11 && mOccupationBoard.contains(Coordinates(x + 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x + 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x + 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (x > 0 && mOccupationBoard.contains(Coordinates(x - 1, y, z)) && (z > 3 || !mOccupationBoard.contains(Coordinates(x - 1, y, z + 1))))
		{
			auto index = mOccupationBoard[Coordinates(x - 1, y, z)];
			if (!Removable[index]) RemovableWasFalse.emplace_back(index);
			Removable[index] = true;
		}
		if (z > 0) // mOccupationBoard[Coordinates(x, y, z-1)] DOIT exister.
		{
			if (x < 11 && !mOccupationBoard.contains(Coordinates(x + 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
			if (x > 0 && !mOccupationBoard.contains(Coordinates(x - 1, y, z - 1)))
			{
				auto index = mOccupationBoard[Coordinates(x, y, z - 1)];
				if (!Removable[index]) RemovableWasFalse.emplace_back(index);
				Removable[index] = true;
			}
		}
	}
}

inline void BuildMoves(const std::vector<DominoIndex>& RemovableBoard, std::vector<DominoIndex>::const_iterator& itFirst, std::vector<std::vector<int>>& Moves)
{
	if (itFirst != RemovableBoard.end())
	{
		auto domino = (*itFirst).domino;
		auto itNext = itFirst;
		std::vector<int> temp;
		do
		{
			++itNext;
			itNext = std::find_if(itNext, RemovableBoard.end(),
				[domino](const DominoIndex& in)
				{
					return
						(
							in.domino == domino ||
							(34 <= in.domino && in.domino < 38 && 34 <= domino && domino < 38) || // Saisons
							(38 <= in.domino && in.domino < 42 && 38 <= domino && domino < 42) // Fleurs.
							);
				}
			);
			if (itNext != RemovableBoard.end())
			{
				if (temp.empty())
					temp.emplace_back(itFirst->index);
				temp.emplace_back(itNext->index);
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

inline void SetMoves(const std::vector<DominoIndex>& LogicalBoard, const std::array<bool, 144>& Removable, std::vector<std::vector<int>>& Moves)
{
	std::vector<DominoIndex> RemovableBoard; // (domino, index)
	for (const auto& pair : LogicalBoard)
	{
		if (Removable[pair.index]) RemovableBoard.emplace_back(pair);
	}

	std::sort(RemovableBoard.begin(), RemovableBoard.end(), [](const DominoIndex& left, const DominoIndex& right)
		{
			return left.domino < right.domino;
		});
	auto itFirst = RemovableBoard.begin();
	Moves.clear();
	BuildMoves(RemovableBoard, itFirst, Moves);
}

static std::map<uint64_t, std::array<uint64_t, 144 / 8>> hashtable;

#define FNV_offset_basis 0xcbf29ce484222325ULL
#define FNV_prime 0x100000001b3ULL

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
inline uint64_t getFNV1(const std::map<int, int>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
	}

	uint64_t hash = FNV_offset_basis;

	for(int i = 0; i < 144; ++i)
	{
		hash *= FNV_prime;
		hash ^= tileTab[i];
	}

	return hash;
}

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
inline uint64_t getFNV1a(const std::map<int, int>& TilesMap)
{
	uint64_t tileTab[144];
	memset(tileTab, 44, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
	}

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}
	
	return hash;
}

inline bool stopNow( const std::map<int, int>& TilesMap 
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{

	uint64_t tileTab[144];
	memset(tileTab, 0, 144 * sizeof(uint64_t));

	for (auto& item : TilesMap)
	{
		tileTab[item.first] = uint64_t(item.second);
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

	uint64_t hash = FNV_offset_basis;
	for (int i = 0; i < 144; ++i)
	{
		hash ^= tileTab[i];
		hash *= FNV_prime;
	}

	if (hashtable.contains(hash))
	{
		auto boardDescriptionInHashtable = hashtable[hash];
		for (int i = 0; i < 144 / 8; ++i)
		{
			if (boardDescriptionInHashtable[i] != boardDescription[i])
			{
#ifdef _DEBUG
				std::cout << "Collision de hash, " << positions << std::endl;
#endif
				hashtable[hash] = boardDescription;
				return false;
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
	std::vector<DominoIndex> & LogicalBoard,
	std::array<bool, 144> & Removable,
	std::map<int, int> & TilesMap,
	std::vector<int> & WhatsLeft,
	std::map<Coordinates, int> & mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution
#ifdef _DEBUG
	, uint64_t& positions
#endif
)
{

	std::vector<std::vector<int>> newMoves;

	std::vector<DominoIndex> LogicalBoardBack;
	std::vector<int> RemovableWasTrue;
	std::vector<int> RemovableWasFalse;
	std::map<int, int> TilesMapBack;
	std::vector<int> WhatsLeftBack;
	std::map<Coordinates, int> mOccupationBoardBack;
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

	auto ret = LogicalBoard.empty();
	if (!ret)
	{
		if (!newMoves.empty())
		{
			if (!stopNow(TilesMap
#ifdef _DEBUG
				, positions
#endif
			))
			{
				for (auto& move : newMoves)
				{
					ret = SolveRec(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution
#ifdef _DEBUG
						, ++positions
#endif
					);

					if (ret) break;
				}
			}
		}
		if (!ret)
		{
			if (full)
				Solution.pop_back();
			Solution.pop_back();
		}
	}
	/**/
	for (auto& item : LogicalBoardBack) LogicalBoard.emplace_back(item);
	for (auto& item : RemovableWasTrue) Removable[item] = true;
	for (auto& item : RemovableWasFalse) Removable[item] = false;
	for (auto& item : TilesMapBack) TilesMap[item.first] = item.second;
	for (auto& item : WhatsLeftBack) WhatsLeft.emplace_back(item);
	for (auto& item : mOccupationBoardBack) mOccupationBoard[item.first] = item.second;
	/**/

	return ret;
}

// Just to work on a copy.
inline bool SolveRecInit(
	std::vector<std::pair<int, int>> oldMoves,
	std::vector<DominoIndex> LogicalBoard,
	std::array<bool, 144> Removable,
	std::map<int, int> TilesMap,
	std::vector<int> WhatsLeft,
	std::map<Coordinates, int> mOccupationBoard,
	std::vector<std::pair<int, int>>& Solution)
{
#ifdef _DEBUG
	std::vector<DominoIndex> LogicalBoardBack = LogicalBoard;
	std::array<bool, 144> RemovableBack = Removable;
	std::map<int, int> TilesMapBack = TilesMap;
	std::vector<int> WhatsLeftBack = WhatsLeft;
	std::map<Coordinates, int> mOccupationBoardBack = mOccupationBoard;
#endif

	Solution.clear();
	// Must be cleared. The hash just means the positions has been seen at least once before. It doesn't mean it was in a lost game.
	// And the start position is always different.
	hashtable.clear();
	bool ret = false;

	// New move container to remove the tiles 2 at once or 4 at once.
	std::vector<std::vector<int>> Moves;
	for(int index = 0; index < oldMoves.size();)
	{
		std::vector<int> temp;
		temp.emplace_back(oldMoves[index].first);
		temp.emplace_back(oldMoves[index].second);
		if (oldMoves.size() >= (index + 6) && oldMoves[index].first == oldMoves[index + 1].first && oldMoves[index].first == oldMoves[index + 2].first)
		{
			temp.emplace_back(oldMoves[index + 5].first);
			temp.emplace_back(oldMoves[index + 5].second);
			index += 6;
		}
		else
		{
			++index;
		}
		Moves.emplace_back(temp);
	}
#ifdef _DEBUG
	uint64_t positions = 0ULL;
#endif

	for (auto& move : Moves)
	{
		ret = SolveRec(move, LogicalBoard, Removable, TilesMap, WhatsLeft, mOccupationBoard, Solution
#ifdef _DEBUG
			, positions
#endif
		);
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
