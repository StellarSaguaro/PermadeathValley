/*
 *  Pawn Class
 */

#ifndef __PAWN_HH__
#define __PAWN_HH__

#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "rogrand.hh"
#include "gameboard.hh"
#include "tile.hh"
#include "navigator.hh"

using namespace rogrand;

class Gameboard;
class Tile;

class Pawn
{
private:
    // Is this the player pawn?
    bool isPlayer;

    // Pawn Location
    bLoc wPos;  // World position
    bLoc prevPos;

    SDL_Texture* pawnTexture;

protected:
    // Board info for player
    Gameboard* mBoard;

    // Does this pawn do anything?
    bool isActive;

    // Queued Path
    vector<bLoc> myPath;

    // ID for Pawn type      TODO: Evaluate the best way to track NPC types (i.e. cactus, cow, bandit, etc);
    unsigned char pawnType;

    // Life Points (i.e. Hit Points)
    bool vulnerable;
    signed int lp;

    // Experience Points
    int xp;
    int kills;

public:
    // Constructor & Destructor
    Pawn( Gameboard*, int, int );
    ~Pawn();

    // Mutators
    void      setPlayer();
    void      setLoc( int, int );
    void      setBoard( Gameboard* );
    DIRECTION moveDir(int);
    DIRECTION moveTo(bLoc);
    DIRECTION moveTo(int toX, int toY, bool dmgMove=true);
    void      moveBack();
    void      setLP(int);
    bool      setActive(bool inAct=true);
    int       addXP(int);
    int       dealDmg(Tile*, int);
    int       takeDmg(int);
    void      setVulnerable(bool);
    void      setTexture(SDL_Texture*);

    // Accessors
    bool        getPlayer()             { return isPlayer; };
    bLoc        getWorldPos()           { return wPos; };
    int         getWorldX()             { return wPos.x; };
    int         getWorldY()             { return wPos.y; };

    // TODO: try default args to remove multiple functions
    bLoc        getBoardPos();
    bLoc        getBoardPos(bLoc inPos);
    int         getBoardX();
    int         getBoardX(int inX);
    int         getBoardY();
    int         getBoardY(int inY);

    bool        isVulnerable()          { return vulnerable; };
    signed int  getLP()                 { return lp; };
    int         getXP()                 { return xp; };
    bool        getActive()             { return isActive; };
    int         getKills()              { return kills; };
    SDL_Texture* getTexture()           { return pawnTexture; };

    // Return Pawn type (for rendering, etc.)
    unsigned char getType() { return pawnType; };
};

class NPC : public Pawn
{
private:
    // Is this NPC hostile?
    bool isHostile;

    // Probability for  NPC to move
    double moveProb;

public:
    // Constructor & Destructor
    NPC(Gameboard* inBoard, int initX, int initY, unsigned char npcT='c', bool isHstl=false, double moveP=0.0);
    ~NPC();

    // Accessors and Mutators
    void setMoveProb(double);
    double getMoveProb();

    // "Do your thing" (whatever this NPC does)
    void dyt(bLoc);
};

#endif
// EOF
