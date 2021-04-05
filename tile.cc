/*
 *  Tile Class
 */

#include "tile.hh"

Tile::Tile( int x, int y, int elv )
{
    elev     = elv;
    std::fill_n(crnrElev, 4, elev); // This may not be portable... Use for loop instead?
    terrain  = 'd';
    occupied = false;
    isFresh  = true;
    tPos     = bLoc{x,y};
//    fovPos   = tPos;
    myPawn   = nullptr;

    updateFlags();
}

Tile::~Tile()
{
    // Nothing to do for now...
}

void Tile::updateFlags()
{
    if (myPawn!=nullptr) {
        if (!myPawn->getPlayer()) { // Allows NPCs to target player
            occupied = true;
        }
    }
    else {
        occupied = false;
    }

    // TODO: Remove magic numbers
    switch (elev) {
        case 0: // Deep Water
        case 4: // Mountain
            occupied = true;
            break;
        default:
            break;
    }

    isFresh=true;
}

void Tile::setTerrain( unsigned char terrType )
{
    terrain = terrType;
    updateFlags();
}

unsigned char Tile::getTerrain()
{
    return terrain;
}

void Tile::setElev( int elv )
{
    elev = elv;
    std::fill_n(crnrElev, 4, elev); // This may not be portable... Use for loop instead?
    updateFlags();
}

int Tile::getElev()
{
    return elev;
}

void Tile::setCrnr( int cnr, int cnrElv )
{
    crnrElev[cnr] = cnrElv;
    updateFlags();
}

int Tile::getCrnr( int cnr )
{
    return crnrElev[cnr];
}

bLoc Tile::getWorldPos()
{
    return tPos;
}

int Tile::getWorldX()
{
    return tPos.x;
}

int Tile::getWorldY()
{
    return tPos.y;
}

void Tile::setTexture(SDL_Texture* tTxtr)
{
    tileTexture = tTxtr;
}

//void Tile::setFOVPos(bLoc fPos)
//{
//    fovPos = fPos;
//
//    if (hasPawn()) {
//        getPawn()->setFOVPos(fovPos);
//    }
//}
//
//bLoc Tile::getFOVPos()
//{
//    return fovPos;
//}
//
//int Tile::getFOVX()
//{
//    return fovPos.x;
//}
//
//int Tile::getFOVY()
//{
//    return fovPos.y;
//}

bool Tile::getOccupied()
{
    return occupied;
}

void Tile::setFresh()
{
    isFresh=true;
}

void Tile::setFresh(bool newFresh)
{
    isFresh=newFresh;
}

bool Tile::getFresh()
{
    return isFresh;
}

void Tile::setPawn(Pawn* inPawn)
{
    myPawn   = inPawn;
    updateFlags();
}

Pawn* Tile::getPawn()
{
    return myPawn;
}

bool Tile::hasPawn()
{
    return (myPawn!=nullptr);
}

void Tile::rmvPawn()
{
    myPawn   = nullptr;
    updateFlags();
}

void Tile::toPrint()
{
    printf("INFO: Tile:toPrint()\n");
    printf("        Position  = [%2d,%2d]\n",tPos.x, tPos.y);
//    printf("        FOV Posn  = [%2d,%2d]\n",fovPos.x, fovPos.y);
    printf("        Elevation = %2d\n",elev);
    printf("        Terrrain  = %c\n",terrain);
    printf("        Occupied  = %1d\n",occupied);
}

// EOF
