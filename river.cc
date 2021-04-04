/*
 *  River Class
 */

#include "river.hh"

River::River(DIRECTION brdr)
{
    border = brdr;

    switch (border) {
        case NORTH:
        case SOUTH:
            // Pick a random location along the NORTH/SOUTH border
            setMouth(randI(0,bCols-1));
            break;
        case EAST:
        case WEST:
            // Pick a random location along the EAST/WEST border
            setMouth(randI(0,bRows-1));
            break;
        case CENTER:
            // Pick a random location on the board (with some buffer from edges for aesthetic reasons)
            setMouth(bLoc{bCols/2,bRows/2});
        default:
            break;}
}

River::River(DIRECTION brdr, int indx)
{
    border = brdr;

    setMouth(indx);
}

River::~River()
{
    // Nothing to do for now...
}

DIRECTION River::getBorder()
{
    return border;
}

bLoc River::getMouth()
{
    return mouth;
}

void River::setMouth(bLoc mouthLoc)
{
    mouth = mouthLoc;
}

void River::setMouth(int indx)
{
    switch (border) {
        default:
        case NORTH:
            mouth = bLoc{indx,0};
            break;
        case SOUTH:
            mouth = bLoc{indx,bRows-1};
            break;
        case EAST:
            mouth = bLoc{bCols-1,indx};
            break;
        case WEST:
            mouth = bLoc{0,indx};
            break;}
}

// EOF
