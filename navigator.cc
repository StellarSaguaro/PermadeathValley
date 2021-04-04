/*
 *  Navigator Class
 */

#include "navigator.hh"
#include "gameboard.hh"

using namespace std;

// == operator overload for bLoc
bool operator==(const bLoc& lhs, const bLoc& rhs) {
    return ((lhs.x==rhs.x) && (lhs.y==rhs.y)); }

// != operator overload for bLoc
bool operator!=(const bLoc& lhs, const bLoc& rhs) {
    return ((lhs.x!=rhs.x) || (lhs.y!=rhs.y)); }

// + operator overload for bLoc
bLoc operator+(const bLoc& lhs, const bLoc& rhs) {
    return (bLoc){(lhs.x+rhs.x),(lhs.y+rhs.y)}; }

// - operator overload for bLoc
bLoc operator-(const bLoc& lhs, const bLoc& rhs) {
    return (bLoc){(lhs.x-rhs.x),(lhs.y-rhs.y)}; }

// Comparison for std::priority_queue
struct CmpNodeDist {
    bool operator()(const PNode* lhs, const PNode* rhs) const {
        return lhs->getDist() > rhs->getDist(); }
};

int REVERSE(int fwdDir)
{
    switch(fwdDir) {
        case SW:
            return NE;
            break;
        case SOUTH:
            return NORTH;
            break;
        case SE:
            return NW;
            break;
        case WEST:
            return EAST;
            break;
        case CENTER:
            return CENTER;
            break;
        case EAST:
            return WEST;
            break;
        case NW:
            return SE;
            break;
        case NORTH:
            return SOUTH;
            break;
        case NE:
            return SW;
            break;
        default:
            return NODIR;
            break;
    }
};

vector<bLoc> findPath(bLoc here, bLoc there, Gameboard* pBrd, double wtMult)
{
    //printf("\n--------\nDEBUG: findPath BEGIN...\n"); fflush(stdout);
    vector<bLoc> pathLocs;   // vector of all locations along path

    // Already at the desired destination
    if (here==there) {
        // Return the current loc if this should be a "wait" operation
        //  Could return an empty vector if nothing should be done
        pathLocs.push_back(here);
        return pathLocs; }

    double distMax = 1.0*pBrd->getRows()*pBrd->getCols();
    vector<PNode*> kNodes;     // known/visited nodes
    std::priority_queue<PNode*, vector<PNode*>, CmpNodeDist> uNodes;    // unknown/unvisited nodes
    bool pathFound = false;

    // Create an array for the node status (No Status=0, Unknown=1, Known=2)
    int nodeStat[pBrd->getRows()][pBrd->getCols()];
    for (int jj=0; jj<pBrd->getRows(); jj++) {
        for (int ii=0; ii<pBrd->getCols(); ii++) {
            nodeStat[jj][ii] = 0;
        }
    }

    // Add the "here" location to the KNOWN nodes
    kNodes.push_back(new PNode(here,here,0.0));
    nodeStat[here.y][here.x] = 2;

    // Check and set distances for all neighbors of the current node
    bLoc nebLoc;
    vector<bLoc> deltaLocs   = { bLoc{-1,-1}, bLoc{ 0,-1}, bLoc{ 1,-1},
                                 bLoc{-1, 0},              bLoc{ 1, 0},
                                 bLoc{-1, 1}, bLoc{ 0, 1}, bLoc{ 1, 1} };

    double dMult = 1.0;
    int    tElev = 0;
    double tDist = distMax;
    for (vector<bLoc>::iterator iLoc=deltaLocs.begin(); iLoc!=deltaLocs.end(); iLoc++) {
        nebLoc = kNodes.back()->nPos+(*iLoc);
        //printf("DEBUG: findPath checking neighbor at [%2d,%2d]\n",nebLoc.x,nebLoc.y);

        if ( (nebLoc.x >= 0) && (nebLoc.x < pBrd->getCols()) &&
             (nebLoc.y >= 0) && (nebLoc.y < pBrd->getRows()) ) {
            dMult = 1.0;
            tElev = pBrd->getTile(nebLoc.y,nebLoc.x)->getElev();
            if (tElev <= 1) {
                dMult = pow(wtMult,0);
            }
            else {
                dMult = pow(wtMult,(tElev-1));
            }

            tDist = distMax;
            if ((0==(*iLoc).x)||(0==(*iLoc).y)) {   // N,E,S,W
                tDist=1.0*dMult;
            }
            else {                                  // NE,SE,SW,NW
                tDist=1.4*dMult;
            }

            if (!pBrd->getTile(nebLoc.y,nebLoc.x)->getOccupied() || dMult>1.01) {    // Avoid floating point equality comparison at 1.0
                //printf("DEBUG: findPath UKNOWN node added at [%2d,%2d]\n",nebLoc.x,nebLoc.y);
                uNodes.push(new PNode((bLoc){nebLoc.x,nebLoc.y},
                                      kNodes.back()->nPos,
                                      kNodes.back()->nDist+tDist));
                                      //kNodes.back()->nDist+tDist+manhattan_dist(nebLoc,there)));
                nodeStat[nebLoc.y][nebLoc.x] = 1;
            }
        }
    }

    // Step through the nodes to find shortest path
    int    pathIter  = 0;
    while ( (pathFound == false) && (uNodes.size()>0) && (pathIter<1000000) ) {
        // Add the "visited" node to the KNOWN nodes
        kNodes.push_back(uNodes.top());
        nodeStat[uNodes.top()->nPos.y][uNodes.top()->nPos.x] = 2;

        // Pop the "visited" node from the UNKNOWN nodes
        uNodes.pop();

        // Check if we've arrived at the destination
        if (there==kNodes.back()->nPos) {
            //printf("DEBUG: findPath found destination [%2d,%2d], d=%4.2f!\n",kNodes.back()->nPos.x,kNodes.back()->nPos.y,kNodes.back()->nDist); fflush(stdout);
            pathFound = true;
        }
        else {  // Else, continue to check/add neighbors
            for (vector<bLoc>::iterator iLoc=deltaLocs.begin(); iLoc!=deltaLocs.end(); iLoc++) {
                nebLoc = kNodes.back()->nPos+(*iLoc);
                //printf("DEBUG: findPath checking neighbor at [%2d,%2d]\n",nebLoc.x,nebLoc.y);

                if ( (nebLoc.x >= 0) && (nebLoc.x < pBrd->getCols()) &&
                     (nebLoc.y >= 0) && (nebLoc.y < pBrd->getRows()) ) {
                    dMult = 1.0;
                    tElev = pBrd->getTile(nebLoc.y,nebLoc.x)->getElev();
                    if (tElev <= 1) {
                        dMult = pow(wtMult,0);
                    }
                    else {
                        dMult = pow(wtMult,(tElev-1));
                    }

                    tDist = distMax;
                    if ((0==(*iLoc).x)||(0==(*iLoc).y)) {   // N,E,S,W
                        tDist=1.0*dMult;
                    }
                    else {                                  // NE,SE,SW,NW
                        tDist=1.4*dMult;
                    }

                    if (!pBrd->getTile(nebLoc.y,nebLoc.x)->getOccupied() || dMult>1.01) {   // Avoid floating point equality comparison at 1.0
                        if (0==nodeStat[nebLoc.y][nebLoc.x]) {
                            //printf("DEBUG: findPath UKNOWN node added at [%2d,%2d]\n",nebLoc.x,nebLoc.y);
                            uNodes.push(new PNode((bLoc){nebLoc.x,nebLoc.y},
                                                  kNodes.back()->nPos,
                                                  kNodes.back()->nDist+tDist));
                                                  //kNodes.back()->nDist+tDist+manhattan_dist(nebLoc,there)));
                            nodeStat[nebLoc.y][nebLoc.x] = 1;
                        }
                    }
                }
            }
        }

        pathIter++;
    }
    //printf("DEBUG: pathFind iterations = %4d\n",pathIter); fflush(stdout);

    if (pathFound) {
        //printf("DEBUG: pathFind destination distance = %f\n", kNodes.back()->nDist); fflush(stdout);

        // Build pathLocs vector here...
        bLoc pLoc = there;
        while  (pLoc != here) {
            for (vector<PNode*>::iterator iNode=kNodes.begin();iNode!=kNodes.end();iNode++) {
                if ( (*iNode)->nPos == pLoc ){
                    //printf("DEBUG: pathFind pLoc = [%2d,%2d]\n",pLoc.x,pLoc.y); fflush(stdout);
                    pathLocs.push_back((*iNode)->nPos);
                    pLoc = (*iNode)->pPos;
                }
            }
        }
    }
    else {
        //printf("DEBUG: navigator::findPath No path found.\n"); fflush(stdout);
    }

    return pathLocs;
};

PNode::PNode(bLoc loc, bLoc prev, double dist)
{
    nPos  = loc;
    pPos  = prev;
    nDist = dist;
}

PNode::~PNode()
{

}

Navigator::Navigator()
{

}

Navigator::~Navigator()
{

}

// EOF
