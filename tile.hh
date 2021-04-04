/*
 *  Tile Class
 */

/*
 *  TERRAIN TYPES:
 *
 *  d   :   desert
 *  ?   :   TBD
 *  ?   :   TBD
 *
 *
 */

#ifndef __TILE_HH__
#define __TILE_HH__

#include <SDL2/SDL.h>

#include <string>
#include <algorithm>

#include "pawn.hh"
#include "navigator.hh"

class Pawn;

class Tile
{
private:
    // Member variables
    int             elev;
    int             crnrElev[4];
    unsigned char   terrain;
    bool            occupied;
    bool            isFresh;
    bLoc            tPos;
//    bLoc            fovPos;
    Pawn*           myPawn;

    SDL_Texture* tileTexture;

public:
    //Constuctor & Destructor
    Tile( int, int, int );
    ~Tile();

    // Check/evaluate tile flags
    void updateFlags();

    // Set Tile Terrain Type
    void setTerrain(unsigned char);
    unsigned char getTerrain();

    void  setElev(int);
    int   getElev();
    void  setCrnr(int,int);
    int   getCrnr(int);
    bLoc  getPos();
    int   getX();
    int   getY();
//    void  setFOVPos(bLoc);
//    bLoc  getFOVPos();
//    int   getFOVX();
//    int   getFOVY();
    bool  getOccupied();
    void  setFresh();
    void  setFresh(bool);
    bool  getFresh();
    void  setPawn( Pawn* );
    Pawn* getPawn();
    bool  hasPawn();
    void  rmvPawn();
    void  toPrint();

    void  setTexture(SDL_Texture*);
    SDL_Texture* getTexture() { return tileTexture; };
};

#endif
// EOF
