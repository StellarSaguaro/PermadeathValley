/*
 *  Navigator Class
 */

#ifndef __NAVIGATOR_HH__
#define __NAVIGATOR_HH__

#include <vector>
#include <algorithm>
//#include <cfloat>
#include <cmath>
#include <queue>

using namespace std;

class Gameboard;

enum DIRECTION {
    SW     = 1,
    SOUTH  = 2,
    SE     = 3,
    WEST   = 4,
    CENTER = 5,
    EAST   = 6,
    NW     = 7,
    NORTH  = 8,
    NE     = 9,
    NODIR  = 10 };

enum INTERCARDINAL {
    NE_IC  = 0,
    SE_IC  = 1,
    SW_IC  = 2,
    NW_IC  = 3,
    MAX_IC = 4 };

// Board Location struct
struct bLoc {
    int x;
    int y; };

int REVERSE(int);

vector<bLoc> findPath(bLoc here, bLoc there, Gameboard* pBrd, double wtMult=1.0);

// == operator overload for bLoc
bool operator==(const bLoc& lhs, const bLoc& rhs);

// != operator overload for bLoc
bool operator!=(const bLoc& lhs, const bLoc& rhs);

// + operator overload for bLoc
bLoc operator+(const bLoc& lhs, const bLoc& rhs);

// - operator overload for bLoc
bLoc operator-(const bLoc& lhs, const bLoc& rhs);

// Calculate Manhattan distance between locations
inline double manhattan_dist(bLoc loca, bLoc locb) {
  return std::abs(loca.x - locb.x) + std::abs(loca.y - locb.y); }

// Calculate Euclidean distance between locations
inline double euclidean_dist(bLoc loca, bLoc locb) {
  return pow(pow(std::abs(loca.x-locb.x),2.0) + pow(std::abs(loca.y-locb.y),2.0),0.5); }

class PNode
{
    private:

    public:

    // Constructor and Destructor
    PNode(bLoc, bLoc, double);
    ~PNode();

    // Methods
    double getDist() const {return nDist;};
    bLoc   getPos()  const {return nPos;};
    bLoc   getPrev() const {return pPos;};

    // Node Properties
    bLoc   nPos;    // this Node's position
    bLoc   pPos;    // previous Node's position
    double nDist;
};

class Navigator
{
    private:

    public:

    // Constructor & Destructor
    Navigator();
    ~Navigator();

    // Navigation Methods

};

#endif
// EOF
