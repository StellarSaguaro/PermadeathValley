/*
 *  Main Program
 *
 */

#include <SDL2/SDL.h>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <random>

#ifdef _WIN32
#include <windows.h>
#endif

#include "rogrand.hh"
#include "gamemaster.hh"
#include "pawn.hh"
#include "navigator.hh"

using namespace rogrand;

/*
 *  Main Program Loop
 */
int main( int argc, char* args[] )
{
    // Setup the random seed for this game
    printf("GAME SEED: %16u\n",seed); fflush(stdout);

    // Instantiate the Gamemaster (i.e. Dungeon Master)
    Gamemaster* DM = new Gamemaster();

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event evnt;

    // Add Player to game
    Pawn* player1 = DM->addPlayer();

    // Initial rendering of game board
    DM->renderBoard();

    // Move list for "click to move"
    vector<bLoc> moveQ;

    //While application is running
    while( (player1->getLP()>0) &&
           ((!moveQ.empty()) || (!quit && SDL_WaitEvent(&evnt))) )
    {
        bool updtBoard = false;

        // Come up with a random direction in case needed later
        //int randDir = randI10();

        // Variable to capture board move direction
        DIRECTION boardDir = NODIR;

        if (!moveQ.empty()) {
            int toDir = player1->moveTo(moveQ.back().x,moveQ.back().y,false);
            if (NODIR==toDir) {
                moveQ.clear();  // We bumped into something, stop moving
            }
            else {
                updtBoard = true;
                moveQ.pop_back();
#ifdef _WIN32
                Sleep(20);
#elif  __linux__
                usleep(20000);
#endif
            }
        }
        else {
            switch ( evnt.type ) {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch ( evnt.key.keysym.sym ) {
                        case SDLK_KP_8:
                        case SDLK_UP:
                            boardDir = player1->moveDir(NORTH);
                            break;
                        case SDLK_KP_2:
                        case SDLK_DOWN:
                            boardDir = player1->moveDir(SOUTH);
                            break;
                        case SDLK_KP_4:
                        case SDLK_LEFT:
                            boardDir = player1->moveDir(WEST);
                            break;
                        case SDLK_KP_6:
                        case SDLK_RIGHT:
                            boardDir = player1->moveDir(EAST);
                            break;
                        case SDLK_KP_7:
                            boardDir = player1->moveDir(NW);
                            break;
                        case SDLK_KP_9:
                            boardDir = player1->moveDir(NE);
                            break;
                        case SDLK_KP_3:
                            boardDir = player1->moveDir(SE);
                            break;
                        case SDLK_KP_1:
                            boardDir = player1->moveDir(SW);
                            break;
                        case SDLK_KP_5:
                        case SDLK_SPACE:
                            boardDir = player1->moveDir(CENTER);
                            break;
                        case SDLK_KP_0:
                        case SDLK_BACKSPACE:
                            DM->deletePlayer();
                            player1 = nullptr;
                            player1 = DM->addPlayer();
                            break;
                        case SDLK_KP_PERIOD:
                        case SDLK_PERIOD:
                            DM->toPrint();
                            printf("Remaining health = %4d\n\n",player1->getLP());
                            printf("Total Experience = %4d\n\n",player1->getXP());
                            DM->swapRenderMode();
                            break;
                    }
                    //printf("DEBUG: Board move direction (%2d).\n",boardDir); fflush(stdout);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    // TODO: this math doesn't work when rendering FOV

                    ////printf("DEBUG: Mouse Click! [%2d,%2d]\n",(evnt.button.x/DM->getTileSize()),(evnt.button.y/DM->getTileSize())); fflush(stdout);
                    bLoc tempLoc = {(evnt.button.x/DM->getTileSize()),(evnt.button.y/DM->getTileSize())};
                    moveQ = findPath(player1->getBoardPos(), tempLoc, DM->getBoard());
                    break;
            }
        }

        if (NODIR != boardDir) {
            updtBoard = true;
        }

        // Set board based on player movement
        if (!(boardDir==CENTER || boardDir==NODIR)) {
            //printf("DEBUG: Moving in direction %2d\n",boardDir); fflush(stdout);
            DM->moveToBoard(boardDir);
        }

        // Update all actors on DM's current board
        // Render board with updated player position
        if (updtBoard) {
            DM->update();
            DM->renderBoard();
        }
    }

    DM->toPrint();

    if (player1->getLP()<=0) {
        printf("You died! ");
    }
    printf("Remaining health = %4d\n\n",player1->getLP());
    printf("Foes Vanquished  = %4d\n\n",player1->getKills());
    printf("Total Experience = %4d\n\n",player1->getXP());

    //Free resources and close SDL
    //printf("DEBUG: Delete DM.\n");
    delete DM;

    printf("Adios!\n"); fflush(stdout);
    return EXIT_SUCCESS;
}

// EOF
