#include "Benchmark.h"

void TestEngine()
{
	unsigned int resolus = 0;

	auto globalstart{ std::chrono::steady_clock::now() };

	auto start{ std::chrono::steady_clock::now() };
	auto end{ std::chrono::steady_clock::now() };

	double duree_min = DBL_MAX;
	double duree_max = -DBL_MAX;

	double echec_duree_min = DBL_MAX;
	double echec_duree_max = -DBL_MAX;

	std::vector<unsigned int> vEchecs;

	for (int i = 0; i < 10000; ++i)
	{
		Board plateau;
		plateau.InitBoard();
		std::vector<std::pair<int, int>> vSolution;
#ifdef _DEBUG
		uint64_t positions = 0ULL;
#endif
		start = std::chrono::steady_clock::now();
		std::future<bool> solver = std::async(
			&SolveRecAsyncInit, plateau.GetMovesLeft(), plateau.getLogicalBoard(), plateau.getRemovable(), plateau.getTilesMap(), plateau.getOccupationBoard(), std::ref(vSolution)
#ifdef _DEBUG
			, positions
#endif
		);
		if (solver.get())
		{
			end = std::chrono::steady_clock::now();
			++resolus;
			std::chrono::duration<double> elapsed_seconds{end - start};
			std::cout << "Réussite en " << elapsed_seconds.count() << " secondes." << std::endl;
			duree_min = std::min(duree_min, elapsed_seconds.count());
			duree_max = std::max(duree_max, elapsed_seconds.count());
		}
		else
		{
			end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds{end - start};
			const auto Seed = plateau.getSeed();
			vEchecs.emplace_back(Seed);
			std::cout << "Echec : " << std::dec << Seed << ". Durée : " << elapsed_seconds.count() << " secondes." << std::endl;
			echec_duree_min = std::min(echec_duree_min, elapsed_seconds.count());
			echec_duree_max = std::max(echec_duree_max, elapsed_seconds.count());
		}
	}

	auto globalend{ std::chrono::steady_clock::now() };

	std::cout << "Nombre d'échecs : " << std::dec << vEchecs.size() << "." << std::endl;
	std::cout << "Echec de : ";
	for (auto itEchec = vEchecs.begin(); itEchec != (vEchecs.end() - 1); ++itEchec)
		std::cout << *itEchec << ", ";
	std::cout << vEchecs.back() << "." << std::endl;

	std::chrono::duration<double> elapsed_seconds{globalend - globalstart};
	std::cout << "Durée totale : " << elapsed_seconds.count() << " secondes." << std::endl;

	std::cout << "Durée minimale : " << duree_min << " secondes." << std::endl;
	std::cout << "Durée maximale : " << duree_max << " secondes." << std::endl;

	std::cout << "Durée échec minimale : " << echec_duree_min << " secondes." << std::endl;
	std::cout << "Durée échec maximale : " << echec_duree_max << " secondes." << std::endl;
}

bool TestLocked()
{
	std::vector<std::pair<int, int>> Locked;
	std::map<int, int> causes;
	int cause;
	for (int i = 0; i < 100000; ++i)
		//int i = 2;
	{
		//InitBoardLockedHorizontal(i);
		Board plateau;
		plateau.InitBoard();
		if (CheckIfLockedFromStart(plateau.getLogicalBoard(), plateau.getTilesMap(), &cause))
		{
			Locked.emplace_back(std::make_pair(i, cause));
			causes[cause] += 1;
		}
	}

	std::cout << std::endl;

	if (!Locked.empty())
	{
		auto it = Locked.begin();
		for (; it != Locked.end() - 1; ++it)
		{
			std::cout << std::dec << "(" << it->first << ";" << it->second << "), ";
		}
		std::cout << std::dec << "(" << it->first << ";" << it->second << ")." << std::endl;
	}

	std::cout << "Bloqués : " << std::dec << Locked.size() << "." << std::endl;

	for (const auto& cause : causes)
		std::cout << "Cause : (" << std::dec << cause.first << "; " << cause.second << ")." << std::endl;

	return false;
}

bool Test()
{
	uint64_t ret = 0ULL;
	int solved = 0;
	std::stringstream strout;
	std::vector<int> Locked;
	int i = 0;
	bool goOn = true;
	do
	{
		int cause;
		Board plateau;
		plateau.InitBoard();
		if (CheckIfLockedFromStart(plateau.getLogicalBoard(), plateau.getTilesMap(), &cause))
		{
			Locked.emplace_back(i++);
			continue;
		}
		auto temp = testAll(plateau);
		if (temp != 0ULL)
			++solved;
		std::cout << std::dec << i << " : " << std::hex << temp << std::dec << std::endl;
		ret += temp;
		auto v9 = temp & 0b01111111;
		temp = temp >> 7;
		auto v8 = temp & 0b01111111;
		temp = temp >> 7;
		auto v7 = temp & 0b01111111;
		temp = temp >> 7;
		auto v6 = temp & 0b01111111;
		temp = temp >> 7;
		auto v5 = temp & 0b01111111;
		temp = temp >> 7;
		auto v4 = temp & 0b01111111;
		temp = temp >> 7;
		auto v3 = temp & 0b01111111;
		temp = temp >> 7;
		auto v2 = temp & 0b01111111;
		temp = temp >> 7;
		auto v1 = temp & 0b01111111;
		temp = temp >> 7;

		strout << std::endl;
		strout << std::dec << v1 << ", ";
		strout << std::dec << v2 << ", ";
		strout << std::dec << v3 << ", ";
		strout << std::dec << v4 << ", ";
		strout << std::dec << v5 << ", ";
		strout << std::dec << v6 << ", ";
		strout << std::dec << v7 << ", ";
		strout << std::dec << v8 << ", ";
		strout << std::dec << v9 << std::endl;
		++i;

		auto tempRet = ret;
		for (int dec = 0; dec < 9 && goOn; ++dec)
		{
			auto rv1 = tempRet & 0b01111111;
			tempRet = tempRet >> 7;
			if (rv1 == 0b01111111)
				goOn = false;
		}
	} while (goOn);

	std::cout << strout.str();

	auto v9 = ret & 0b01111111;
	ret = ret >> 7;
	auto v8 = ret & 0b01111111;
	ret = ret >> 7;
	auto v7 = ret & 0b01111111;
	ret = ret >> 7;
	auto v6 = ret & 0b01111111;
	ret = ret >> 7;
	auto v5 = ret & 0b01111111;
	ret = ret >> 7;
	auto v4 = ret & 0b01111111;
	ret = ret >> 7;
	auto v3 = ret & 0b01111111;
	ret = ret >> 7;
	auto v2 = ret & 0b01111111;
	ret = ret >> 7;
	auto v1 = ret & 0b01111111;
	ret = ret >> 7;

	std::cout << std::endl;
	std::cout << std::dec << v1 << ", ";
	std::cout << std::dec << v2 << ", ";
	std::cout << std::dec << v3 << ", ";
	std::cout << std::dec << v4 << ", ";
	std::cout << std::dec << v5 << ", ";
	std::cout << std::dec << v6 << ", ";
	std::cout << std::dec << v7 << ", ";
	std::cout << std::dec << v8 << ", ";
	std::cout << std::dec << v9 << std::endl;

	auto pourcent = solved * 100. / (i - Locked.size());

	std::cout << "Résolus : " << std::dec << pourcent << "%" << std::endl;

	std::cout << std::endl;

	if (!Locked.empty())
	{
		auto it = Locked.begin();
		for (; it != Locked.end() - 1; ++it)
		{
			std::cout << std::dec << *it << ", ";
		}
		std::cout << std::dec << *it << std::endl;
	}
	std::cout << "Bloqués : " << std::dec << Locked.size() << "." << std::endl;

	return false;
}

int main()
{
	Test();
	TestLocked();
	TestEngine();
}
