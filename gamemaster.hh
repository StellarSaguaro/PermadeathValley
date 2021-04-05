/*
 *  Gamemaster Class
 */

#ifndef __GAMEMASTER_HH__
#define __GAMEMASTER_HH__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "rogrand.hh"
#include "gameboard.hh"
#include "navigator.hh"
#include "tile.hh"
#include "pawn.hh"

using namespace std;
using namespace rogrand;

//Screen dimension constants
static const int WINDOW_HEIGHT = 960;
static const int WINDOW_WIDTH  = WINDOW_HEIGHT*1.0;

// World Size
static const int wRows = 128;
static const int wCols = 128;
static const int wRadius = sqrt(((wRows/2)^2)+((wCols/2)^2));

// Board Size
static const int bRows = 64;
static const int bCols = 64;
static const int bRadius = sqrt(((bRows/2)^2)+((bCols/2)^2));

class Gamemaster
{
private:
    // Turn counter
    int turnCount;

    //Tile size (side length in pixels)
    static const int tileSizeBOARD = WINDOW_HEIGHT/bRows;
    int tileSize = tileSizeBOARD;

    // Sprite Textures
    static const int numTxtrs = 5;      // Total number of terrain textures
    SDL_Texture* txtrTerr[numTxtrs];    // Terrain Textures
    SDL_Texture* txtrCrnr[numTxtrs*MAX_IC];  // Corner Textures (4 per terrain level)

    SDL_Texture* txtrDesert;
    SDL_Texture* txtrMesa;
    SDL_Texture* txtrAgua;
    SDL_Texture* txtrCowboy;
    SDL_Texture* txtrBandit;
    SDL_Texture* txtrCactus[3];
    SDL_Texture* txtrGila;
    SDL_Texture* txtrCow;
    SDL_Texture* txtrError;

    // FOV vs. Board Render Flag
    //  true = only render field of view (vRows x vCols)
    //  false = render full board (bRows x bCols)
    bool RENDER_FOV = false;

public:
    // SDL window and renderer
    SDL_Window*     gWindow = nullptr;
    SDL_Renderer* gRenderer = nullptr;

    //Constructor & Destructor
    Gamemaster();
    ~Gamemaster();

    // Initialize Gamemaster
    int init();

    //Render Objects
    void renderTile( Tile* );
    void renderPawn( Pawn* );
    int  getTileSize();
    void swapRenderMode();

    // Gamemaster Methods
    Gameboard* addBoard(int, int);
    int        moveToBoard(DIRECTION);
    void       setBoard(Gameboard*);
    Gameboard* getBoard();
    void       renderBoard();
    void       renderBoard(Gameboard*);
    void       toPrint();
    void       update();
    bool       saveBoardBMP(Gameboard*, char*);

    // Player Methods
    Pawn* addPlayer();
    void  deletePlayer();

    // Game Board
    Gameboard* worldBoard;
    Gameboard* currBoard;
    bLoc       wPos;
    vector<Gameboard*>           mBoard;
    vector<Gameboard*>::iterator iBoard;

    //Player Pawn
    Pawn* player;

    //Current NPCs
    vector<NPC*>* currNPCs;
};

#endif
// EOF
