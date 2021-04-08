/*
 *  Gameboard Class
 */

#include "gameboard.hh"

Gameboard::Gameboard(int locX, int locY, int inRows, int inCols, Gameboard* inWorld)
{
    //printf("DEBUG: Gameboard::Gameboard Creating new board at [%2d,%2d].\n", locX, locY);

    bPos.x = locX;
    bPos.y = locY;

    bRows = inRows;
    bCols = inCols;
    bRadius = sqrt(((bRows/2)^2)+((bCols/2)^2));

    board = vector<vector<Tile*>>(bRows, vector<Tile*>(bCols,nullptr));

    if (nullptr != inWorld)
    {
        // Initialize all board tiles based on section of world
        for (int jj=0; jj<bRows; jj++)
        {
            for (int ii=0; ii<bCols; ii++)
            {
                board[jj][ii] = inWorld->getTile((bRows*locY)+jj,
                                                 (bCols*locX)+ii);
            }
        }

        placeEntities();
    }
    else
    {
        // Initialize world tiles and create map
        for (int jj=0; jj<bRows; jj++)
        {
            for (int ii=0; ii<bCols; ii++)
            {
                board[jj][ii] = new Tile(ii, jj, 2);
            }
        }

        createMap();
    }
}

Gameboard::~Gameboard()
{
    // Delete Tiles
    //printf("DEBUG: Begin Gameboard destructor.\n");

    for (int jj=0; jj<bRows; jj++)
    {
        for (int ii=0; ii<bCols; ii++)
        {
            delete board[jj][ii];
            board[jj][ii] = nullptr;
        }
    }

    while (!bNPCs.empty())
    {
        bNPCs.pop_back();
    }

    //printf("DEBUG: End Gameboard destructor.\n");
}

bool Gameboard::createMap()
{
    // FastNoiseLite Implementation
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(0.06f);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
    noise.SetFractalOctaves(4);

    double dvsr = 2.29928*log(0.0337477*bRows);  // log fit {120,3.3},{240,4.8},{320,5.2},{480,6.6}
    //printf("DEBUG: Elevation Adjustment Divisor = %4.2g\n",dvsr); fflush(stdout);

    // Create the tile map using noise
    for (int jj=0; jj<bRows; jj++)
    {
        for (int ii=0; ii<bCols; ii++)
        {
            double dElev = ((noise.GetNoise((double)jj,(double)ii)+1.0)/2.0)*elevMax;
            // printf("DEBUG: Elevation = %2.4g\n",dElev); fflush(stdout);

            /*
            ** Use a power function to increase elevation further away from
            ** map center to create "valley". Clamp to max elevation.
            */
            double dist = sqrt( pow((jj-(bRows/2)),2) + pow((ii-(bCols/2)),2) );
            dElev += pow(double(dist)/double(bRadius)/dvsr,15.0);
            dElev = min((int)dElev, elevMax);

            //if      ( dElev >= threshT08*elevMax ) { board[jj][ii]->setElev(8); }
            //else if ( dElev >= threshT07*elevMax ) { board[jj][ii]->setElev(7); }
            //else if ( dElev >= threshT06*elevMax ) { board[jj][ii]->setElev(6); }
            //else if ( dElev >= threshT05*elevMax ) { board[jj][ii]->setElev(5); }
            if      ( dElev >= threshT04*elevMax ) { board[jj][ii]->setElev(4); }
            else if ( dElev >= threshT03*elevMax ) { board[jj][ii]->setElev(3); }
            else if ( dElev >= threshT02*elevMax ) { board[jj][ii]->setElev(2); }
            else if ( dElev >= threshT01*elevMax ) { board[jj][ii]->setElev(1); }
            else                                   { board[jj][ii]->setElev(0); }
        }
    }

    /*
    ** Generate River(s)
    */

    // Push all directions into available rivers list
    riversAvail.push_back(NORTH);
    riversAvail.push_back(EAST);
    riversAvail.push_back(SOUTH);
    riversAvail.push_back(WEST);

    // Pick all river starting locations (directional borders)
    for (int rr=0; rr<numRivers; rr++)
    {
        std::shuffle(std::begin(riversAvail), std::end(riversAvail), mt);

        switch (riversAvail.back())
        {
            case NORTH:
                mRivers.push_back(bLoc{randI(0,bCols-1),0});
                break;
            case EAST:
                mRivers.push_back(bLoc{bCols-1,randI(0,bRows-1)});
                break;
            case SOUTH:
                mRivers.push_back(bLoc{randI(0,bCols-1),bRows-1});
                break;
            case WEST:
                mRivers.push_back(bLoc{bRows-1,randI(0,bRows-1)});
                break;
            default:
                mRivers.push_back(bLoc{0,0});
                break;
        }

        // Pop off back of available rivers if rivers 
        // from the same direction are undesired
        riversAvail.pop_back();
    }

    vector<bLoc> masterRvrQ;
    masterRvrQ.push_back(bLoc{bCols/2,bRows/2});
    for (vector<bLoc>::iterator iRvr=mRivers.begin(); iRvr!=mRivers.end(); iRvr++)
    {
        // findPath for rivers is weighted to help river flow through lowest elevation path
        //printf("DEBUG: Creating River from [%2d,%2d]\n",(*iRvr).x,(*iRvr).y); fflush(stdout);
        vector<bLoc> rvrQ = findPath(bLoc{bCols/2,bRows/2},*iRvr,this,rvrElevWeight);
        masterRvrQ.insert(masterRvrQ.end(),rvrQ.begin(),rvrQ.end());
        masterRvrQ.push_back(*iRvr);
    }
    double rvrW; // Vary river width
    while (!masterRvrQ.empty()) {
        rvrW = randI(0,riverWidth)+riverWidth;
        for (int dy=ceil(-rvrW/2.0); dy<ceil(rvrW/2.0); dy++)
        {
            for (int dx=ceil(-rvrW/2.0); dx<ceil(rvrW/2.0); dx++)
            {
                if(board[std::max(0, std::min(masterRvrQ.back().y+dy, bRows-1))]
                     [std::max(0, std::min(masterRvrQ.back().x+dx, bCols-1))]->getElev() < 4)       // TODO: get rid of magic number (max elev)
                {
                    int newElev = board[std::max(0, std::min(masterRvrQ.back().y+dy, bRows-1))]
                                       [std::max(0, std::min(masterRvrQ.back().x+dx, bCols-1))]->getElev();
                    newElev = max(newElev-randI(0,1),0);

                    board[std::max(0, std::min(masterRvrQ.back().y+dy, bRows-1))]
                         [std::max(0, std::min(masterRvrQ.back().x+dx, bCols-1))]->setElev(randI(0,1));
                         //[std::max(0, std::min(masterRvrQ.back().x+dx, bCols-1))]->setElev(1);
                }
            }
        }
        masterRvrQ.pop_back();
    }

    return true;
}

void Gameboard::placeEntities()
{
    // Run through all board tiles for final touches
    for (int jj=0; jj<bRows; jj++)
    {
        for (int ii=0; ii<bCols; ii++)
        {
            // Add some NPCs
            // TODO: Consider moving NPC population into a separate handler class (i.e. pupeteer?)
            // TODO: Remove magic numbers for elevation checks
            //printf("DEBUG: Adding NPCs...\n"); fflush(stdout);

            //if (board[jj][ii]->getElev() == 4) {
            //    // Place a mountain here
            //    bNPCs.push_back(new NPC(this, ii, jj, 'm', false, 0.0));
            //    //bNPCs.back()->setVulnerable(false);   // Make mountains invincible
            //    bNPCs.back()->setLP(1);
            //    board[jj][ii]->setPawn(bNPCs.back());
            //}; else
            if ( (board[jj][ii]->getElev() == 2) || (board[jj][ii]->getElev() == 3) )
            {
                int wPosX = ii+(bPos.x*bCols);
                int wPosY = jj+(bPos.y*bRows);

                if (randI1000()<20)
                {
                    // Place a cactus here
                    bNPCs.push_back(new NPC(this, wPosX, wPosY, 'c', false, 0.0));
                    board[jj][ii]->setPawn(bNPCs.back());
                }
                else if (randI1000()<2)
                {
                    int randTmp = randI1000();
                    if (randTmp<900)
                    {
                        // Place a cow here
                        bNPCs.push_back(new NPC(this, wPosX, wPosY, 'w', false, 0.9));
                        board[jj][ii]->setPawn(bNPCs.back());
                    }
                    else
                    {
                        // Place a gila monster here
                        bNPCs.push_back(new NPC(this, wPosX, wPosY, 'g', true, 0.7));
                        board[jj][ii]->setPawn(bNPCs.back());
                    }
                }
            }
        }
    }
}

Tile* Gameboard::getTile(int row, int col)
{
    return board[row][col];
}

vector<NPC*>* Gameboard::getNPCs()
{
    return &bNPCs;
}

void Gameboard::checkNPCs(bLoc playerPos)
{
    // Check/Update the status of all NPCs on the board
    for (vector<NPC*>::iterator iNPC=bNPCs.begin(); iNPC!=bNPCs.end();)
    {
        // Remove NPCs with with no life
        if ((*iNPC)->getLP() <= 0)
        {
            //printf("DEBUG: Gameboard::checkNPCs removing pawn %1c from board\n",(*iNPC)->getType()); fflush(stdout);
            getTile((*iNPC)->getBoardY(),(*iNPC)->getBoardX())->rmvPawn();
            iNPC = bNPCs.erase(iNPC);
        }
        else
        {
            // Check/update isActive status (only execute if the NPC actually moves)
            // TODO: refine criteria for activate/inactivate NPCs (dist from player?)
            if ( (*iNPC)->getMoveProb() > 1e-6 )
            {
                //printf("DEBUG: Gameboard::checkNPCs distance of %1c from player is %6.2f\n",(*iNPC)->getType(),euclidean_dist(playerPos, (*iNPC)->getBoardPos())); fflush(stdout);
                if ( euclidean_dist(playerPos, (*iNPC)->getBoardPos()) < (bRows/2) ) {
                    (*iNPC)->setActive(); }
                else {
                    (*iNPC)->setActive(false); }
            }

            ++iNPC;
        }
    }
}

// EOF
