/*
 *  Random Helper
 */

#ifndef __ROGRAND_HH__
#define __ROGRAND_HH__

#include <random>

namespace rogrand {
    // Global random number generator (TODO: remove this from global)
    extern unsigned int seed;
    extern std::mt19937 mt;
    extern std::uniform_int_distribution<int> dist2;
    extern std::uniform_int_distribution<int> dist10;
    extern std::uniform_int_distribution<int> dist100;
    extern std::uniform_int_distribution<int> dist1000;

    int randI10();
    int randI100();
    int randI1000();
    int randI(int, int);
}

#endif
// EOF
