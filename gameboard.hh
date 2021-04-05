/*
 *  Gameboard Class
 *
 *
 *  NOTE: The gameboard dimensions are defined as follows:
 *
 *  [ [0,0] [0,1] [0,2] ... [0,N]
 *    [1,0] [1,1] [1,2] ... [1,N]
 *    [2,0] [2,1] [2,2] ... [2,N]
 *    ...   ...   ...   ... ...
 *    [N,0] [N,1] [N,2] ... [N,N] ]
 *
 */

#ifndef __GAMEBOARD_HH__
#define __GAMEBOARD_HH__

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

#include "rogrand.hh"
#include "navigator.hh"
#include "tile.hh"
#include "pawn.hh"

#include "FastNoiseLite.h"

using namespace rogrand;

class River;
class Tile;
class Pawn;
class NPC;

class Gameboard
{
private:
    // Terrain params
    int    elevMax   = 1000;    // Maximum random elevation
    double threshT00 = 0.000;   // Terrain threshold to be elevation 0, Deep Water
    double threshT01 = 0.010;   // Terrain threshold to be elevation 1, Shallow Water
    double threshT02 = 0.020;   // Terrain threshold to be elevation 2, Sand
    double threshT03 = 0.450;   // Terrain threshold to be elevation 3, Dirt
    double threshT04 = 0.820;   // Terrain threshold to be elevation 4, Mesa
    //double threshT05 = 0.820;   // Terrain threshold to be elevation 5, Grass
    //double threshT06 = 0.940;   // Terrain threshold to be elevation 6, Forest
    //double threshT07 = 0.960;   // Terrain threshold to be elevation 7, Mountain
    //double threshT08 = 0.980;   // Terrain threshold to be elevation 8, Snow

    // Board Location in game "world"
    bLoc bPos;

    // Board Size
    int bRows;
    int bCols;
    int bRadius;

    // Tile Matrix
    vector<vector<Tile*>> board;

    // River Data
    vector<DIRECTION> riversAvail;  // Directions available for river mouths
    vector<bLoc> mRivers;           // River mouths on this board
    double rvrElevWeight = 10.0;    // Terrain weighting for finding river paths
    int numRivers = 2;              // Total # of rivers (flowing to center)
    int riverWidth = 3;             // River width and variation

    // Create Elevation and Tile maps
    bool createMap();

    // Place flora and fauna
    void placeEntities();

    // Vector of all Pawns on the board
    // TODO: implement ACTIVE and INACTIVE NPC vectors
    vector<NPC*> bNPCs;

public:

    //Constructor & Destructor
    Gameboard(int locX, int locY, int nRows, int nCols, Gameboard* inWorld=nullptr);
    ~Gameboard();

    //Accessor Methods
    bLoc  getBoardPos() {return bPos;};
    int   getWorldX()   {return bPos.x;};
    int   getWorldY()   {return bPos.y;};
    int   getRows()     {return bRows;};
    int   getCols()     {return bCols;};
    Tile* getTile(int, int);

    vector<NPC*>* getNPCs();
    void          checkNPCs(bLoc);
    //vector<NPC*>* addNPC( NPC*, int, int );         // TODO: implement this...
    //vector<NPC*>* rmvNPC(vector<NPC*>::iterator);   // TODO: implement this...
};

#endif
// EOF
