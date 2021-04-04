/*
 *  Gameboard Class
 */

#include "gameboard.hh"

Gameboard::Gameboard(int locX, int locY, Gameboard* inWorld)
{
    //printf("DEBUG: Gameboard::Gameboard Creating new board at [%2d,%2d].\n", locX, locY);

    bPos.x = locX;
    bPos.y = locY;

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
        for (int jj=0; jj<wRows; jj++)
        {
            for (int ii=0; ii<wCols; ii++)
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
        mRivers.push_back(new River(riversAvail.back()));

        // Pop off back of available rivers if rivers 
        // from the same direction are undesired
        riversAvail.pop_back();
    }

    vector<bLoc> masterRvrQ;
    masterRvrQ.push_back(bLoc{bCols/2,bRows/2});
    for (vector<River*>::iterator iRvr=mRivers.begin(); iRvr!=mRivers.end(); iRvr++)
    {
        //printf("DEBUG: Creating River from %2d\n",(*iRvr)->getBorder()); fflush(stdout);
        // findPath for rivers is weighted to help river flow through lowest elevation path
        vector<bLoc> rvrQ = findPath(bLoc{bCols/2,bRows/2},(*iRvr)->getMouth(),this,rvrElevWeight);
        masterRvrQ.insert(masterRvrQ.end(),rvrQ.begin(),rvrQ.end());
        masterRvrQ.push_back((*iRvr)->getMouth());
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
/*
            // Check/Set terrain tile corners
            //printf("DEBUG: Checking Corners...\n"); fflush(stdout);
            if ((jj>0) && (jj<(bRows-1)) && (ii>0) && (ii<(bCols-1)))
            {

                //// Check/Set NE Corner
                //if ( (board[jj-1][ii+1]->getElev() == board[jj][ii+1]->getElev() ) &&
                //     (board[jj-1][ii+1]->getElev() == board[jj-1][ii]->getElev() ) )
                //{
                //    board[jj][ii]->setCrnr(NE_IC,board[jj-1][ii+1]->getElev());
                //}

                //// Check/Set SE Corner
                //if ( (board[jj+1][ii+1]->getElev() == board[jj][ii+1]->getElev() ) &&
                //     (board[jj+1][ii+1]->getElev() == board[jj+1][ii]->getElev() ) )
                //{
                //    board[jj][ii]->setCrnr(SE_IC,board[jj+1][ii+1]->getElev());
                //}

                //// Check/Set SW Corner
                //if ( (board[jj+1][ii-1]->getElev() == board[jj][ii-1]->getElev() ) &&
                //     (board[jj+1][ii-1]->getElev() == board[jj+1][ii]->getElev() ) )
                //{
                //    board[jj][ii]->setCrnr(SW_IC,board[jj+1][ii-1]->getElev());
                //}

                //// Check/Set NW Corner
                //if ( (board[jj-1][ii-1]->getElev() == board[jj][ii-1]->getElev() ) &&
                //     (board[jj-1][ii-1]->getElev() == board[jj-1][ii]->getElev() ) )
                //{
                //    board[jj][ii]->setCrnr(NW_IC,board[jj-1][ii-1]->getElev());
                //}
// above logic only updates corners when surrounded on 3 sides by same elev
// below logic updates corners if 2 adjacent same elevs but NOT caddy corner
                // Check/Set NE Corner
                if ( (board[jj-1][ii+1]->getElev() != board[jj][ii]->getElev() ) &&
                     (board[jj][ii+1]->getElev()   == board[jj-1][ii]->getElev() ) )
                {
                    board[jj][ii]->setCrnr(NE_IC,board[jj][ii+1]->getElev());
                }

                // Check/Set SE Corner
                if ( (board[jj+1][ii+1]->getElev() != board[jj][ii]->getElev() ) &&
                     (board[jj][ii+1]->getElev()   == board[jj+1][ii]->getElev() ) )
                {
                    board[jj][ii]->setCrnr(SE_IC,board[jj][ii+1]->getElev());
                }

                // Check/Set SW Corner
                if ( (board[jj+1][ii-1]->getElev() != board[jj][ii]->getElev() ) &&
                     (board[jj][ii-1]->getElev()   == board[jj+1][ii]->getElev() ) )
                {
                    board[jj][ii]->setCrnr(SW_IC,board[jj][ii-1]->getElev());
                }

                // Check/Set NW Corner
                if ( (board[jj-1][ii-1]->getElev() != board[jj][ii]->getElev() ) &&
                     (board[jj][ii-1]->getElev()   == board[jj-1][ii]->getElev() ) )
                {
                    board[jj][ii]->setCrnr(NW_IC,board[jj][ii-1]->getElev());
                }
            }
*/

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
                if (randI1000()<20)
                {
                    // Place a cactus here
                    bNPCs.push_back(new NPC(this, ii, jj, 'c', false, 0.0));
                    board[jj][ii]->setPawn(bNPCs.back());
                }
                else if (randI1000()<20)
                {
                    int randTmp = randI1000();
                    if (randTmp<900)
                    {
                        // Place a cow here
                        bNPCs.push_back(new NPC(this, ii, jj, 'w', false, 0.9));
                        board[jj][ii]->setPawn(bNPCs.back());
                    }
                    else
                    {
                        // Place a gila monster here
                        bNPCs.push_back(new NPC(this, ii, jj, 'g', true, 0.7));
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
            getTile((*iNPC)->getY(),(*iNPC)->getX())->rmvPawn();
            iNPC = bNPCs.erase(iNPC);
        }
        else
        {
            // Check/update isActive status (only execute if the NPC actually moves)
            // TODO: refine criteria for activate/inactivate NPCs (dist from player?)
            if ( (*iNPC)->getMoveProb() > 1e-6 )
            {
                //printf("DEBUG: Gameboard::checkNPCs distance of %1c from player is %6.2f\n",(*iNPC)->getType(),euclidean_dist(playerPos, (*iNPC)->getPos())); fflush(stdout);
                if ( euclidean_dist(playerPos, (*iNPC)->getPos()) < (vRows/2) ) {
                    (*iNPC)->setActive(); }
                else {
                    (*iNPC)->setActive(false); }
            }

            ++iNPC;
        }
    }
}


// RMV /*
// RMV ** --------------------
// RMV ** Worldboard
// RMV ** ----------
// RMV */
// RMV 
// RMV Worldboard::Worldboard()
// RMV     :Gameboard(nullptr, -1,-1)
// RMV {
// RMV     //printf("DEBUG: Worldboard::Worldboard Creating new board.\n");
// RMV 
// RMV     // Initialize all board tiles
// RMV     for (int jj=0; jj<wRows; jj++)
// RMV     {
// RMV         for (int ii=0; ii<wCols; ii++)
// RMV         {
// RMV             wBoard[jj][ii] = new Tile(ii, jj, 2);
// RMV         }
// RMV     }
// RMV 
// RMV     createMap();
// RMV }
// RMV 
// RMV Worldboard::~Worldboard()
// RMV {
// RMV     // Delete Tiles
// RMV     //printf("DEBUG: Begin Worldboard destructor.\n");
// RMV 
// RMV     for (int jj=0; jj<wRows; jj++)
// RMV     {
// RMV         for (int ii=0; ii<wCols; ii++)
// RMV         {
// RMV             delete wBoard[jj][ii];
// RMV             wBoard[jj][ii] = nullptr;
// RMV         }
// RMV     }
// RMV 
// RMV     //printf("DEBUG: End Worldboard destructor.\n");
// RMV }
// RMV 
// RMV bool Worldboard::createMap()
// RMV {
// RMV     // FastNoiseLite Implementation
// RMV     FastNoiseLite noise;
// RMV     noise.SetSeed(seed);
// RMV     noise.SetFrequency(0.06f);
// RMV     noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
// RMV     noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes);
// RMV     noise.SetFractalOctaves(4);
// RMV 
// RMV     double dvsr = 2.29928*log(0.0337477*wRows);  // log fit {120,3.3},{240,4.8},{320,5.2},{480,6.6}
// RMV     //printf("DEBUG: Elevation Adjustment Divisor = %4.2g\n",dvsr); fflush(stdout);
// RMV 
// RMV     // Create the tile map using noise
// RMV     for (int jj=0; jj<wRows; jj++)
// RMV     {
// RMV         for (int ii=0; ii<wCols; ii++)
// RMV         {
// RMV             double dElev = ((noise.GetNoise((double)jj,(double)ii)+1.0)/2.0)*elevMax;
// RMV             // printf("DEBUG: Elevation = %2.4g\n",dElev); fflush(stdout);
// RMV 
// RMV             /*
// RMV             ** Use a power function to increase elevation further away from
// RMV             ** map center to create "valley". Clamp to max elevation.
// RMV             */
// RMV             double dist = sqrt( pow((jj-(wRows/2)),2) + pow((ii-(wCols/2)),2) );
// RMV             dElev += pow(double(dist)/double(wRadius)/dvsr,15.0);
// RMV             dElev = min((int)dElev, elevMax);
// RMV 
// RMV             //if      ( dElev >= threshT08*elevMax ) { wBoard[jj][ii]->setElev(8); }
// RMV             //else if ( dElev >= threshT07*elevMax ) { wBoard[jj][ii]->setElev(7); }
// RMV             //else if ( dElev >= threshT06*elevMax ) { wBoard[jj][ii]->setElev(6); }
// RMV             //else if ( dElev >= threshT05*elevMax ) { wBoard[jj][ii]->setElev(5); }
// RMV             if      ( dElev >= threshT04*elevMax ) { wBoard[jj][ii]->setElev(4); }
// RMV             else if ( dElev >= threshT03*elevMax ) { wBoard[jj][ii]->setElev(3); }
// RMV             else if ( dElev >= threshT02*elevMax ) { wBoard[jj][ii]->setElev(2); }
// RMV             else if ( dElev >= threshT01*elevMax ) { wBoard[jj][ii]->setElev(1); }
// RMV             else                                   { wBoard[jj][ii]->setElev(0); }
// RMV         }
// RMV     }
// RMV 
// RMV     /*
// RMV     ** Generate River(s)
// RMV     */
// RMV 
// RMV     // Push all directions into available rivers list
// RMV     riversAvail.push_back(NORTH);
// RMV     riversAvail.push_back(EAST);
// RMV     riversAvail.push_back(SOUTH);
// RMV     riversAvail.push_back(WEST);
// RMV 
// RMV     // Pick all river starting locations (directional borders)
// RMV     for (int rr=0; rr<numRivers; rr++)
// RMV     {
// RMV         std::shuffle(std::begin(riversAvail), std::end(riversAvail), mt);
// RMV         mRivers.push_back(new River(riversAvail.back()));
// RMV 
// RMV         // Pop off back of available rivers if rivers 
// RMV         // from the same direction are undesired
// RMV         riversAvail.pop_back();
// RMV     }
// RMV 
// RMV     vector<bLoc> masterRvrQ;
// RMV     masterRvrQ.push_back(bLoc{wCols/2,wRows/2});
// RMV     for (vector<River*>::iterator iRvr=mRivers.begin(); iRvr!=mRivers.end(); iRvr++)
// RMV     {
// RMV         //printf("DEBUG: Creating River from %2d\n",(*iRvr)->getBorder()); fflush(stdout);
// RMV         // findPath for rivers is weighted to help river flow through lowest elevation path
// RMV         vector<bLoc> rvrQ = findPath(bLoc{wCols/2,wRows/2},(*iRvr)->getMouth(),this,rvrElevWeight);
// RMV         masterRvrQ.insert(masterRvrQ.end(),rvrQ.begin(),rvrQ.end());
// RMV         masterRvrQ.push_back((*iRvr)->getMouth());
// RMV     }
// RMV 
// RMV     double rvrW; // Vary river width
// RMV     while (!masterRvrQ.empty())
// RMV     {
// RMV         rvrW = randI(0,riverWidth)+riverWidth;
// RMV         for (int dy=ceil(-rvrW/2.0); dy<ceil(rvrW/2.0); dy++)
// RMV         {
// RMV             for (int dx=ceil(-rvrW/2.0); dx<ceil(rvrW/2.0); dx++)
// RMV             {
// RMV                 if(wBoard[std::max(0, std::min(masterRvrQ.back().y+dy, wRows-1))]
// RMV                      [std::max(0, std::min(masterRvrQ.back().x+dx, wCols-1))]->getElev() < 4)       // TODO: get rid of magic number (max elev)
// RMV                 {
// RMV                     int newElev = wBoard[std::max(0, std::min(masterRvrQ.back().y+dy, wRows-1))]
// RMV                                        [std::max(0, std::min(masterRvrQ.back().x+dx, wCols-1))]->getElev();
// RMV                     newElev = max(newElev-randI(0,1),0);
// RMV 
// RMV                     wBoard[std::max(0, std::min(masterRvrQ.back().y+dy, wRows-1))]
// RMV                          [std::max(0, std::min(masterRvrQ.back().x+dx, wCols-1))]->setElev(randI(0,1));
// RMV                          //[std::max(0, std::min(masterRvrQ.back().x+dx, wCols-1))]->setElev(1);
// RMV                 }
// RMV             }
// RMV         }
// RMV         masterRvrQ.pop_back();
// RMV     }
// RMV 
// RMV     return true;
// RMV }
// RMV 
// RMV Tile* Worldboard::getTile(int row, int col)
// RMV {
// RMV     return wBoard[row][col];
// RMV }

// EOF
