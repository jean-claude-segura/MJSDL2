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
		stopSolverNow = false;
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

int main()
{
	TestEngine();
}
