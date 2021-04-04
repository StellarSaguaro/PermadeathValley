/*
 *  Gamemaster Class
 */

#include "gamemaster.hh"

Gamemaster::Gamemaster()
{
    init();

    // Create the world and game boards
    worldBoard = new Gameboard(-1,-1);

    for (int jj=0; jj<(wRows/bRows); jj++)
    {
        for (int ii=0; ii<(wCols/bCols); ii++)
        {
            addBoard(ii,jj);
        }
    }

    //setBoard(addBoard(wPos.x, wPos.y));
    setBoard(mBoard.back());

    currNPCs = currBoard->getNPCs();

    // Give pawns their textures
    for (vector<NPC*>::iterator iNPC=currNPCs->begin(); iNPC!=currNPCs->end(); ++iNPC)
    {
        switch ((*iNPC)->getType())
        {
            case '@':
                (*iNPC)->setTexture(txtrCowboy);
                break;
            case 'b':
                (*iNPC)->setTexture(txtrBandit);
                break;
            case 'm':
                (*iNPC)->setTexture(txtrMesa);
                break;
            case 'c':
                (*iNPC)->setTexture(txtrCactus[randI(0,2)]);
                break;
            case 'g':
                (*iNPC)->setTexture(txtrGila);
                break;
            case 'w':
                (*iNPC)->setTexture(txtrCow);
                break;
            default:
                (*iNPC)->setTexture(txtrError);
                break;
        }
    }
}

Gamemaster::~Gamemaster()
{
    //printf("DEBUG: Begin Gamemaster destructor.\n");
    deletePlayer();

    // Delete game board
    for (iBoard=mBoard.begin(); iBoard!=mBoard.end(); iBoard++) {
        delete (*iBoard);
    }
    mBoard.clear();

    // Destroy window
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;
    SDL_DestroyWindow( gWindow );
    gWindow   = nullptr;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    //printf("DEBUG: End Gamemaster destructor.\n");
}

int Gamemaster::init()
{
    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "ERROR: SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

    // Set texture filtering to "nearest" for that pixel-y look
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) ) {
        printf( "Warning: SDL Render Scale Quality method failed!" );
    }

    // Create window
    gWindow = SDL_CreateWindow( "permadeath_valley", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == nullptr ) {
        printf( "ERROR: Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

    // Create renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
    if( gRenderer == nullptr ) {
        printf( "ERROR: Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return EXIT_FAILURE;
    }

    //// Force integer scales for resolution-independent rendering (SDL_TRUE), or don't (SDL_FALSE)
    //SDL_RenderSetIntegerScale(gRenderer,SDL_TRUE);

    // Set renderer blend mode
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

    // Initialize renderer color
    // TODO: make background color same as mountain color
    SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        printf( "ERROR: SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return EXIT_FAILURE;
    }

    //Load Textures
    SDL_Surface* srfcTemp = nullptr;

    // Tile/Terrain Textures (by elevation)
    for (int iT=0; iT<numTxtrs; iT++) {
        char strTerr [256];
        sprintf(strTerr,"images/terrain%02d.png",iT);
        srfcTemp = IMG_Load(strTerr);

        if (nullptr!=srfcTemp) {
            txtrTerr[iT] = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
            SDL_FreeSurface( srfcTemp );
        }
        else {
            printf("ERROR: Unable to load terrain texture.\n");
            printf("       %s\n", strTerr); fflush(stdout);
            exit(EXIT_FAILURE);
        }

        // Load 4 corners for each terrain level
        for (int iC=0; iC<MAX_IC; iC++)
        {
            char strCrnr [256];

            switch (iC) {   // NO default CASE, SHOULD BE UNREACHABLE...
                case NE_IC:
                    sprintf(strCrnr,"images/corner%02d_ne.png",iT);
                    srfcTemp = IMG_Load(strCrnr);
                    break;
                case SE_IC:
                    sprintf(strCrnr,"images/corner%02d_se.png",iT);
                    srfcTemp = IMG_Load(strCrnr);
                    break;
                case SW_IC:
                    sprintf(strCrnr,"images/corner%02d_sw.png",iT);
                    srfcTemp = IMG_Load(strCrnr);
                    break;
                case NW_IC:
                    sprintf(strCrnr,"images/corner%02d_nw.png",iT);
                    srfcTemp = IMG_Load(strCrnr);
                    break;
            }

            if (nullptr!=srfcTemp) {
                //printf("DEBUG: Setting corner texture at index [%2d]\n",iT*MAX_IC+iC); fflush(stdout);
                txtrCrnr[iT*MAX_IC+iC] = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
                SDL_FreeSurface( srfcTemp );
            }
            else {
                printf("ERROR: Unable to load corner texture.\n");
                printf("       %s\n", strCrnr); fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Pawn Textures
    //srfcTemp = IMG_Load( "images/mesa_solid.png" );
    //srfcTemp = IMG_Load( "images/mesa06.png" );
    srfcTemp = IMG_Load( "images/mesa_rand00.png" );
    txtrMesa = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    srfcTemp = IMG_Load( "images/cowboy02.png" ); //red.png" );
    txtrCowboy = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    srfcTemp = IMG_Load( "images/bandit02.png" );
    txtrBandit = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    srfcTemp = IMG_Load( "images/cactus_rand00.png" );
    txtrCactus[0] = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );
    srfcTemp = IMG_Load( "images/cactus_rand01.png" );
    txtrCactus[1] = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );
    srfcTemp = IMG_Load( "images/cactus_rand02.png" );
    txtrCactus[2] = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    srfcTemp = IMG_Load( "images/gila03.png" );
    txtrGila = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    srfcTemp = IMG_Load( "images/cow03.png" );
    txtrCow = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    // Default/Broken Texture
    srfcTemp = IMG_Load( "images/badTexture01.png" );
    txtrError = SDL_CreateTextureFromSurface( gRenderer, srfcTemp );
    SDL_FreeSurface( srfcTemp );

    if (RENDER_FOV) {
        tileSize = tileSizeFOV; }
    else {
        tileSize = tileSizeBOARD; }

    wPos.x    = 0;
    wPos.y    = 0;
    currBoard = nullptr;
    turnCount = 0;

    return EXIT_SUCCESS;
}

int Gamemaster::getTileSize()
{
    return tileSize;
}

void Gamemaster::renderTile( Tile* tile )
{
    if (tile->getFresh() || RENDER_FOV)
    {
        //SDL_Texture* tTxtr = nullptr;
        //if ((tile->getElev()>=0) && (tile->getElev()<numTxtrs))
        //{
        //    tTxtr = txtrTerr[tile->getElev()];
        //}
        //else
        //{
        //    tTxtr = txtrError;
        //}
        SDL_Texture* tTxtr = txtrTerr[tile->getElev()];

        //// DEBUG: For testing tile occupied status
        //if (tile->getOccupied()) {tTxtr = txtrError;}

        if (RENDER_FOV)
        {
            adjX = -player->getX()+(vCols/2);
            adjY = -player->getY()+(vRows/2);
        }

        SDL_Rect tRect;
        tRect = { (tile->getX()+adjX)*tileSize, (tile->getY()+adjY)*tileSize, tileSize, tileSize };
        SDL_RenderCopy( gRenderer, tTxtr, nullptr, &(tRect) );

/*
        // Render tiles's corners
        for (int iC=0; iC<MAX_IC; iC++) {
            //printf("DEBUG: Getting corner texture at index: %2d\n",tile->getElev()*MAX_IC+tile->getCrnr(iC)); fflush(stdout);
            SDL_Texture* cTxtr = txtrCrnr[tile->getCrnr(iC)*MAX_IC+iC];

            SDL_Rect cRect;
            cRect = { (tile->getX()+adjX)*tileSize, (tile->getY()+adjY)*tileSize,tileSize, tileSize };
            SDL_RenderCopy( gRenderer, cTxtr, nullptr, &(cRect) );
        }
*/

        tile->setFresh(false);
    }
}

void Gamemaster::renderPawn( Pawn* pwn )
{
    if (RENDER_FOV)
    {
        adjX = -player->getX()+(vCols/2);
        adjY = -player->getY()+(vRows/2);
    }

    SDL_Rect tRect;
    tRect = { (pwn->getX()+adjX)*tileSize, (pwn->getY()+adjY)*tileSize, tileSize, tileSize };
    SDL_RenderCopy( gRenderer, pwn->getTexture(), nullptr, &(tRect) );
}

Pawn* Gamemaster::addPlayer()
{
    int tmpX, tmpY;
    do
    {
        tmpX = randI(0,bCols-1);
        tmpY = randI(0,bRows-1);
    } while ( currBoard->getTile(tmpY,tmpX)->getOccupied() );

    player = new Pawn( currBoard, tmpX, tmpY );
    player->setPlayer();
    player->setLP(100);

    player->setTexture(txtrCowboy);

    currBoard->getTile(tmpY,tmpX)->setPawn(player);

    return player;
}

void Gamemaster::deletePlayer()
{
    currBoard->getTile(player->getY(),player->getX())->rmvPawn();
    delete player;
    player = nullptr;
}

Gameboard* Gamemaster::addBoard(int toX, int toY)
{
    // TODO: determine if player should gain xp for exploring new boards
    mBoard.push_back(new Gameboard(toX, toY, worldBoard));
    return mBoard.back();
}

int Gamemaster::moveToBoard(DIRECTION bDir)
{
    return CENTER;
}

void Gamemaster::setBoard(Gameboard* toBoard)
{
    currBoard = toBoard;
    wPos      = currBoard->getBoardPos();
    currNPCs  = currBoard->getNPCs();
    if (nullptr!=player)
    {
        player->setBoard(currBoard);
    }
}

Gameboard* Gamemaster::getBoard()
{
    return currBoard;
}

// TODO: get rid of methods accounting for multiple boards (obsolete)
void Gamemaster::renderBoard()
{
    renderBoard(currBoard);
}

void Gamemaster::renderBoard(Gameboard* rndrBoard)
{
    if (RENDER_FOV)
    {
        //Clear screen, render texture, update screen
        //printf("DEBUG: Begin renderBoard. Turn: %4d\n", turnCount);
        SDL_RenderClear( gRenderer );
    }

    if (nullptr != rndrBoard) { // Render a blank board if nullptr is passed
        for (int jj=0; jj<bRows; jj++) {
            for (int ii=0; ii<bCols; ii++) {
                renderTile(rndrBoard->getTile(jj,ii));

                if (rndrBoard->getTile(jj,ii)->hasPawn()) {
                    renderPawn(rndrBoard->getTile(jj,ii)->getPawn());
                }
            }
        }
    }

    SDL_RenderPresent( gRenderer );
    //printf("DEBUG: End renderBoard.\n");
}

void Gamemaster::swapRenderMode()
{
    RENDER_FOV = !RENDER_FOV;

    if (RENDER_FOV)
    {
        tileSize = tileSizeFOV;
        adjX = -player->getX()+(vCols/2);
        adjY = -player->getY()+(vRows/2);
    }
    else
    {
        tileSize = tileSizeBOARD;
        adjX = 0;
        adjY = 0;
    }

    for (int jj=0; jj<bRows; jj++) {
        for (int ii=0; ii<bCols; ii++) {
            currBoard->getTile(jj,ii)->setFresh();
        }
    }
    renderBoard();
}

void Gamemaster::update()
{
    // Remove NPCs with with <=0 life
    currBoard->checkNPCs(player->getPos());

    // Tell each NPC to "do your thing"
    // TODO: Consider adding a method to Gameboard to handle the dyt of its bNPCs
    for (vector<NPC*>::iterator iNPC=currNPCs->begin(); iNPC!=currNPCs->end(); ++iNPC) {
        (*iNPC)->dyt(player->getPos());
    }

    turnCount++;
}

void Gamemaster::toPrint()
{
    int Xmin=0;
    int Xmax=0;
    int Ymin=0;
    int Ymax=0;
    int Xspan=0;
    int Yspan=0;

    // Find Max and Min world locations
    for (iBoard=mBoard.begin(); iBoard!=mBoard.end(); iBoard++) {
        Xmin = std::min(Xmin, (*iBoard)->getBoardX());
        Xmax = std::max(Xmax, (*iBoard)->getBoardX());
        Ymin = std::min(Ymin, (*iBoard)->getBoardY());
        Ymax = std::max(Ymax, (*iBoard)->getBoardY());
    }
    Xspan = Xmax-Xmin+1;
    Yspan = Ymax-Ymin+1;
    //printf("DEBUG: Gamemaster::toPrint Xspan = %4d\n",Xspan);
    //printf("DEBUG: Gamemaster::toPrint Yspan = %4d\n",Yspan); fflush(stdout);

    // Generate a 2D world map array, initialize to 0, and populate with existing boards
    int wMap[Yspan][Xspan];
    for (int ii=0; ii<Yspan; ii++) {
        for (int jj=0; jj<Xspan; jj++) {
             wMap[ii][jj] = 0;
        }
    }

    for (iBoard=mBoard.begin(); iBoard!=mBoard.end(); iBoard++) {
        wMap[(*iBoard)->getBoardY()-Ymin][(*iBoard)->getBoardX()-Xmin] = 1;
    }

    // Print the "world map" to the terminal
    char fName[32];
    //renderBoard(nullptr);
    printf("\nYou explored %3lu boards!\n",(long unsigned int)(mBoard.size()));
    printf("\nWorld Map [%2dx%2d]:\n\n",Xspan,Yspan);
    for (int ii=0; ii<Yspan; ii++) {
        printf("    ");
        for (int jj=0; jj<Xspan; jj++) {
             if (0 == wMap[ii][jj]) {
                printf(" ");
             }
             else if (1 == wMap[ii][jj]) {
                printf("x");
             }

            //printf("DEBUG: world_%04dx%04d.bmp\n",ii,jj);
            sprintf(fName,"world_%04dx%04d.bmp",ii,jj);
            saveBoardBMP(nullptr,fName);
        }
        printf("\n");
    }

    // Save screenshots of the boards
    for (iBoard=mBoard.begin(); iBoard!=mBoard.end(); iBoard++) {
        //printf("DEBUG: world_%04dx%04d.bmp\n",(*iBoard)->getBoardX()-Xmin,(*iBoard)->getBoardY()-Ymin); fflush(stdout);
        sprintf(fName,"world_%04dx%04d.bmp",(*iBoard)->getBoardY()-Ymin,(*iBoard)->getBoardX()-Xmin);
        saveBoardBMP(*iBoard,fName);
    }
    //renderBoard();

    printf("\n"); fflush(stdout);
}

bool Gamemaster::saveBoardBMP(Gameboard* saveBrd, char* filepath)
{
    /*
    ** Thanks, Neil Flodin!
    **  https://stackoverflow.com/questions/20233469/how-do-i-take-and-save-a-bmp-screenshot-in-sdl-2
    */

/*
    // NOTE: THIS FUNCTION DOES NOT WORK WHEN TILES/PAWNS ARE RENDERED RELATIVE TO THE FOV POSITION
    //       TILES/PAWNS MUST BE RENDERED RELATIVE TO THEIR TRUE BOARD POSITION FOR COHERENT IMAGES
    SDL_RenderClear( gRenderer );
    renderBoard(saveBrd);
    SDL_RenderPresent( gRenderer );

    SDL_Surface* saveSurface = NULL;
    SDL_Surface* infoSurface = NULL;
    infoSurface = SDL_GetWindowSurface(gWindow);

    if (infoSurface == NULL) {
        std::cerr << "Failed to create info surface from window in saveBoardBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
    }
    else {
        unsigned char * pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
        if (pixels == 0) {
            std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";
            return false;
        }
        else {
            if (SDL_RenderReadPixels(gRenderer, &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
                std::cerr << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";
                delete[] pixels;
                return false;
            }
            else {
                saveSurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
                if (saveSurface == NULL) {
                    std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
                    delete[] pixels;
                    return false;
                }
                SDL_SaveBMP(saveSurface, filepath);
                SDL_FreeSurface(saveSurface);
                saveSurface = NULL;
            }
            delete[] pixels;
        }
        SDL_FreeSurface(infoSurface);
        infoSurface = NULL;
    }
*/
    return true;
}

// EOF
