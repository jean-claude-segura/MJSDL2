#include "Benchmark.h"

std::atomic<bool> goOn;

// https://www.qtcentre.org/threads/9438-Updating-one-part-of-the-output-(C-Cout)?s=1865f1c0ed777361e7c02cd907a10bfb&p=50243#post50243
void Feedback()
{
	const char* control = "\r";
	const char* cursor = { "\\|/-" };
	int i = 0;
	while (goOn)
	{
		std::cout << control << cursor[i] << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		++i;
		i %= 4;
	}
	std::cout << std::endl;
}

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
			std::cout << "Solved in " << elapsed_seconds.count() << " seconds." << std::endl;
			duree_min = std::min(duree_min, elapsed_seconds.count());
			duree_max = std::max(duree_max, elapsed_seconds.count());
		}
		else
		{
			end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds{end - start};
			const auto Seed = plateau.getSeed();
			vEchecs.emplace_back(Seed);
			std::cout << "Failed : " << std::dec << Seed << ". Compute time : " << elapsed_seconds.count() << " seconds." << std::endl;
			echec_duree_min = std::min(echec_duree_min, elapsed_seconds.count());
			echec_duree_max = std::max(echec_duree_max, elapsed_seconds.count());
		}
	}

	auto globalend{ std::chrono::steady_clock::now() };

	std::cout << "Failures count : " << std::dec << vEchecs.size() << "." << std::endl;
	std::cout << "Failures : ";
	for (auto itEchec = vEchecs.begin(); itEchec != (vEchecs.end() - 1); ++itEchec)
		std::cout << *itEchec << ", ";
	std::cout << vEchecs.back() << "." << std::endl;

	std::chrono::duration<double> elapsed_seconds{globalend - globalstart};
	std::cout << "Total compute time : " << elapsed_seconds.count() << " seconds." << std::endl;

	std::cout << "Minimal compute time for solved : " << duree_min << " seconds." << std::endl;
	std::cout << "Maximal compute time for solved : " << duree_max << " seconds." << std::endl;

	std::cout << "Minimal compute time for failed : " << echec_duree_min << " seconds." << std::endl;
	std::cout << "Maximal compute time for failed : " << echec_duree_max << " seconds." << std::endl;
}

bool TestLocked()
{
	std::vector<std::pair<int, int>> Locked;
	std::map<int, int> causes;
	int cause;
	goOn = true;
	std::thread thrFeedback {Feedback};
	for (int i = 0; i < 1000000; ++i)
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

	goOn = false;
	thrFeedback.join();

	if (!Locked.empty())
	{
		auto it = Locked.begin();
		for (; it != Locked.end() - 1; ++it)
		{
			std::cout << std::dec << "(" << it->first << ";" << it->second << "), ";
		}
		std::cout << std::dec << "(" << it->first << ";" << it->second << ")." << std::endl;
	}

	std::cout << "Blocked : " << std::dec << Locked.size() << "." << std::endl;

	for (const auto& cause : causes)
		std::cout << "Cause : (" << std::dec << cause.first << "; " << cause.second << ")." << std::endl;

	return false;
}

bool TestHeuristics()
{
	int solved = 0;
	std::stringstream strout;
	std::vector<unsigned int> Locked;

	std::array <unsigned int, 9> arrGlobalResult = {};
	const unsigned int TOTAL_ATTEMPTS = 1000;
	for(unsigned int attempts = 0; attempts < TOTAL_ATTEMPTS; ++attempts)
	{
		int cause;
		Board plateau;
		plateau.InitBoard();
		std::array <unsigned int, 9> arrResult = {};
		if (CheckIfLockedFromStart(plateau.getLogicalBoard(), plateau.getTilesMap(), &cause))
		{
			Locked.emplace_back(plateau.getSeed());
			continue;
		}
		auto temp = testAll(plateau, arrResult);
		if (temp )
			++solved;
		std::cout << std::dec << plateau.getSeed() << " : " << std::hex << temp << std::dec << std::endl;

		strout << std::endl;
		for (int i = 0; i < 8; ++i)
		{
			strout << std::dec << arrResult[i] << ", ";
			arrGlobalResult[i] += arrResult[i];
		}
		strout << std::dec << arrResult[8] << std::endl;
		arrGlobalResult[8] += arrResult[8];
	}

	std::cout << strout.str();

	std::cout << std::endl;

	for (int i = 0; i < 8; ++i)
		std::cout << std::dec << arrGlobalResult[i] << ", ";
	std::cout << std::dec << arrGlobalResult[8] << std::endl;

	auto pourcent = solved * 100. / (TOTAL_ATTEMPTS - Locked.size());

	std::cout << "Solved : " << std::dec << pourcent << "%" << std::endl;

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
	std::cout << "Blocked : " << std::dec << Locked.size() << "." << std::endl;

	return false;
}

int main()
{
	TestHeuristics();
	TestLocked();
	TestEngine();
}
