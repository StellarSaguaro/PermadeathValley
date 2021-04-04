/*
 *  Random Helper
 */

#include "rogrand.hh"

#ifdef _WIN32
#include <chrono>
#endif

namespace rogrand
{
#ifdef _WIN32
    unsigned int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 mt(seed);
#elif __linux__
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 mt(seed);
#endif

    std::uniform_int_distribution<int> dist10(0, 9);
    std::uniform_int_distribution<int> dist100(0, 99);
    std::uniform_int_distribution<int> dist1000(0, 999);

    int randI10()   { return dist10(mt); }
    int randI100()  { return dist100(mt); }
    int randI1000() { return dist1000(mt); }

    int randI(int iMin, int iMax)
    {
        std::uniform_int_distribution<int> tmpDist(iMin,iMax);
        return tmpDist(mt);
    }
}

// EOF
