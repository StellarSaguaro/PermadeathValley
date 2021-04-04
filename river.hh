/*
 *  River Class
 */

#ifndef __RIVER_HH__
#define __RIVER_HH__

#include "rogrand.hh"
#include "gameboard.hh"
#include "navigator.hh"

using namespace rogrand;

class River
{
private:
    DIRECTION border;
    bLoc      mouth;

public:
    // Constructor & Destructor
    River(DIRECTION);
    River(DIRECTION, int);
    ~River();

    // Methods
    DIRECTION getBorder();
    bLoc      getMouth();
    void      setMouth(bLoc);
    void      setMouth(int);
};

#endif
// EOF
