/*
 *  Pawn Class
 */

#include "pawn.hh"

Pawn::Pawn( Gameboard* inBoard, int initX, int initY )
{
    //Initialize
    isPlayer  = false;
    isActive  = false;
    pawnType  = '@';
    pawnTexture = nullptr;

    wPos    = bLoc{initX,initY};
    prevPos = bLoc{initX,initY};

    vulnerable = true;
    lp = 1;

    xp = 0;
    kills = 0;

    mBoard = inBoard;

}

Pawn::~Pawn()
{
    // Nothing to do for now...
}

void Pawn::setPlayer()
{
    isPlayer = true;

    //// For Debug purposes...
    //printf("INFO:\nINFO: SETTING PLAYER INVULNERABLE!\nINFO:\n"); fflush(stdout);
    //vulnerable = false;
};

void Pawn::setTexture(SDL_Texture* pTxtr)
{
    pawnTexture = pTxtr;
}

DIRECTION Pawn::moveDir( int direction )
{
    bLoc tmpPos = getBoardPos(wPos);

    switch (direction)
    {
        case SW:
            --tmpPos.x;
            ++tmpPos.y;
            break;
        case SOUTH:
            ++tmpPos.y;
            break;
        case SE:
            ++tmpPos.x;
            ++tmpPos.y;
            break;
        case WEST:
            --tmpPos.x;
            break;
        case CENTER:
            // No change to tmpPos
            break;
        case EAST:
            ++tmpPos.x;
            break;
        case NW:
            --tmpPos.x;
            --tmpPos.y;
            break;
        case NORTH:
            --tmpPos.y;
            break;
        case NE:
            ++tmpPos.x;
            --tmpPos.y;
            break;
        default:
            return NODIR;
            break;
    }

//    // Don't let characters exceed board dimensions
//    if ((tmpPos.x < 0) ||
//        (tmpPos.x > mBoard->getCols()-1) ||
//        (tmpPos.y < 0) ||
//        (tmpPos.y > mBoard->getRows()-1))
//    {
//        return NODIR;
//        //tmpPos.x = max(tmpPos.x,0);
//        //tmpPos.x = min(tmpPos.x,mBoard->getCols()-1);
//        //tmpPos.y = max(tmpPos.y,0);
//        //tmpPos.y = min(tmpPos.y,mBoard->getRows()-1);
//    }

    return ( moveTo( tmpPos.x, tmpPos.y) );
}

DIRECTION Pawn::moveTo(bLoc toPos, bool dmgMove)
{
    return moveTo(toPos.x,toPos.y,dmgMove);
}

DIRECTION Pawn::moveTo( int toX, int toY, bool dmgMove)
{
    //printf("DEBUG: Pawn::moveTo toX = %4d, toY = %4d\n", toX, toY); fflush(stdout);

    if ((getBoardX(wPos.x) == toX) && (getBoardY(wPos.y) == toY)) {
        //printf("DEBUG: Pawn::moveTo Waiting here....\n"); fflush(stdout);
        return CENTER;
    }

    prevPos = wPos;
    mBoard->getTile(getBoardY(),getBoardX())->rmvPawn();

    DIRECTION wDir = NODIR;  // World travel direction
    if (toX < 0) {
        if (toY < 0) {
            wDir = NW;
        }
        else if (toY >= mBoard->getRows()) {
            wDir = SW;
        }
        else {
            wDir = WEST;
        }
    }
    else if (toX >= mBoard->getCols()) {
        if (toY < 0) {
            wDir = NE;
        }
        else if (toY >= mBoard->getRows()) {
            wDir = SE;
        }
        else {
            wDir = EAST;
        }
    }
    else if (toY < 0) {
        wDir = NORTH;
    }
    else if (toY >= mBoard->getRows()) {
        wDir = SOUTH;
    }
    else {
        wDir = CENTER;

        if (dmgMove && mBoard->getTile(toY,toX)->hasPawn())
        {
            printf("INFO: Pawn::moveTo  %1c dealing damage\n",pawnType); fflush(stdout);
            addXP(dealDmg(mBoard->getTile(toY,toX),1));
        }
    }

    //if (!mBoard->getTile(toY,toX)->getOccupied())
    //{
        wPos.x = toX+(mBoard->getCols()*mBoard->getWorldX());
        wPos.y = toY+(mBoard->getRows()*mBoard->getWorldY());
    //}
    //else
    //{
    //    //printf("INFO: Pawn::moveTo Destination occupied.\n"); fflush(stdout);
    //    wDir = NODIR;
    //}

    if (CENTER == wDir)
    {
        mBoard->getTile(getBoardY(),getBoardX())->setPawn(this);
    }

    printf("DEBUG: Pawn::moveTo wPos.x = %4d, wPos.Y = %4d, wDir = %4d\n", wPos.x, wPos.y, wDir); fflush(stdout);
    return wDir;
}

void Pawn::moveBack()
{
    //printf("DEBUG: Pawn::moveBack Prev Pos = [%2d,%2d]\n",prevPos.x,prevPos.y); fflush(stdout);
    wPos = prevPos;
}

void Pawn::setBoard( Gameboard* inBoard )
{
    mBoard = inBoard;
    mBoard->getTile(getBoardY(),getBoardX())->setPawn(this);
}

bLoc Pawn::getBoardPos()
{
    return bLoc{wPos.x-(mBoard->getCols()*mBoard->getWorldX()),
                wPos.y-(mBoard->getRows()*mBoard->getWorldY())};
}
bLoc Pawn::getBoardPos(bLoc inPos)
{
    return bLoc{inPos.x-(mBoard->getCols()*mBoard->getWorldX()),
                inPos.y-(mBoard->getRows()*mBoard->getWorldY())};
}

int Pawn::getBoardX()
{
    return wPos.x-(mBoard->getCols()*mBoard->getWorldX());
};
int Pawn::getBoardX(int inX)
{
    return inX-(mBoard->getCols()*mBoard->getWorldX());
};

int Pawn::getBoardY()
{
    return wPos.y-(mBoard->getRows()*mBoard->getWorldY());
};
int Pawn::getBoardY(int inY)
{
    return inY-(mBoard->getRows()*mBoard->getWorldY());
};

void Pawn::setVulnerable( bool isVuln)
{
    vulnerable = isVuln;
}

void Pawn::setLP(int inLP)
{
    lp = inLP;
}

bool Pawn::setActive(bool inAct)
{
    isActive = inAct;
    return isActive;
}

int Pawn::addXP(int newXP)
{
    //printf("DEBUG: Pawn::addXP adding XP = %3d\n",newXP); fflush(stdout);
    xp = xp+newXP;
    return xp;
}

int Pawn::dealDmg(Tile* dltTile, signed int dltDmg)
{
    int xpGained = dltTile->getPawn()->takeDmg(dltDmg);
    if (xpGained>0) {
        // TODO: Evaluate when kills should be awarded (non-hostile, immobile, etc.)
        kills=kills+1;
    }
    return xpGained;
}

int Pawn::takeDmg(signed int tknDmg)
{
    if ( vulnerable)
    {
        //printf("DEBUG: Pawn::takeDmg Old LP=%2d\n", lp);
        lp = lp-tknDmg;
        //printf("                     New LP=%2d\n", lp);
    }
    if (isPlayer) {
        printf("INFO: Pawn::takeDmg %1c taking damage: %2d, remaining health: %2d\n",pawnType,tknDmg,lp); fflush(stdout);
    }

    if (lp<=0) {
        printf("INFO: Pawn::takeDmg %1c killed. Returning XP = %3d\n",pawnType,xp); fflush(stdout);
        mBoard->checkNPCs(wPos);
        // TODO: Evaluate when xp should be awarded (non-hostile, immobile, etc.)
        return xp;
    }
    else {
        return 0;
    }
}

NPC::NPC( Gameboard* inBoard, int initX, int initY, unsigned char npcT, bool isHstl, double moveP )
    : Pawn(inBoard, initX, initY)
{
    //Initialize
    pawnType   = npcT;
    isHostile = isHstl;
    moveProb  = moveP;
    addXP(5);
}

void NPC::setMoveProb(double inProb)
{
    moveProb = inProb;
}

double NPC::getMoveProb()
{
    return moveProb;
}

void NPC::dyt(bLoc inputPos)
{
    if (isActive)
    {
        if (randI1000()<(moveProb*1000))
        {
            if (isHostile)
            {
                myPath = findPath(getBoardPos(),inputPos,mBoard);    // Try to find a path to the player
            }

            if (myPath.empty())
            {
                // Pick a random location on the map and wander to it
                myPath = findPath(getBoardPos(),
                                  bLoc{randI(0,mBoard->getCols()-1),randI(0,mBoard->getRows()-1)},
                                  mBoard);
            }

            if (!myPath.empty())
            {
                int toDir = moveTo(myPath.back(),isHostile);
                if (NODIR==toDir)
                {
                    myPath.clear(); // bumbed into something, stop moving
                }
                else
                {
                    myPath.pop_back();
                }
            }
        }
    }
}

// EOF
