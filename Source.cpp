#include "Chunk.h"
#include "World.h"
#include "Hash.h"
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector> 
#include "SimpleWorker.h"

using namespace std;

//Screen dimension constants
constexpr static const int32_t SCREEN_WIDTH = 1024;
constexpr static const int32_t SCREEN_HEIGHT = 768;
constexpr static const int32_t RENDER_WIDTH = SCREEN_WIDTH;
constexpr static const int32_t RENDER_HEIGHT = SCREEN_HEIGHT;

constexpr static const char* kWindowTitle = "Sand";


Cell _EMPTY, _SAND, _WATER, _ROCK;

World theWorld = World(64, 64, (SCREEN_WIDTH / 64)-1, (SCREEN_HEIGHT / 64)-1); //holds the world, will need to change parameters 


//function prototypes

SDL_Window* CreateCenteredWindow(); //creates window, returns it

SDL_Renderer* CreateRenderer(SDL_Window* pWindow, bool hardwareAccelerated); //creates renderer

SDL_Texture* CreateBackBufferTexture(SDL_Renderer* pRenderer); //creates texture buffer

void Shutdown(SDL_Window** ppWindow, SDL_Renderer** ppRenderer, SDL_Texture** ppTexture); //shuts down the SDL components

int32_t Startup(SDL_Window** ppWindow, SDL_Renderer** ppRenderer, SDL_Texture** ppTexture); //initializes the SDLL components

bool ProcessInput(); //processes inputs, returns true when exit has been selected

void Update(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture); // the update function, runs every frame in PlayGame

void PlayGame(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture); // runs the game, calls update, processess inputs, and draws the screen

void RunSim(); //runs the sand sim

bool MoveSide(size_t x, size_t y, const Cell& cell); //checks if the object can move to the side, then adds it to the move que if it can

bool MoveDownSide(size_t x, size_t y, const Cell& cell); //checks if the object can move down to the side, then adds it to the move que if it can

bool MoveDown(size_t x, size_t y, const Cell& cell); //checks if the object can move down, then adds it to the move que if it can

void InitializeCells();//sets up the global cells, the types of stuff that will be in the world

void DrawScreen(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture); //draws the screen

void mousePress(SDL_MouseButtonEvent& b, int xt, int yt); //handles mouse press events

void DrawRect(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture, Chunk* chunk); //draws the dirty rects



SDL_Window* CreateCenteredWindow()
{
    // Get current device's Display Mode to calculate window position
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    // Calculate where the upper-left corner of a centered window will be
    const int32_t x = DM.w / 2 - SCREEN_WIDTH / 2;
    const int32_t y = DM.h / 2 - SCREEN_HEIGHT / 2;

    // Create the SDL window
    SDL_Window* pWindow = SDL_CreateWindow(kWindowTitle, x, y, SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_ALLOW_HIGHDPI);

    if (!pWindow)
    {
        cout << "Failed to create Window\n";
    }

    return pWindow;
}

// Create SDL renderer and configure whether or not to use Hardware Acceleration
SDL_Renderer* CreateRenderer(SDL_Window* pWindow, bool hardwareAccelerated)
{
    if (hardwareAccelerated)
        return SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    else
        return SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_SOFTWARE);
}

// Create an SDL Texture to use as a "back buffer"
SDL_Texture* CreateBackBufferTexture(SDL_Renderer* pRenderer)
{
    SDL_Texture* pTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, RENDER_WIDTH, RENDER_HEIGHT);

    if (!pTexture)
    {
        cout << "Failed to create Back Buffer Texture\n";
    }

    return pTexture;
}


// Free resources 
void Shutdown(SDL_Window** ppWindow, SDL_Renderer** ppRenderer, SDL_Texture** ppTexture)
{
    // Free the Back Buffer
    if (ppTexture)
    {
        SDL_DestroyTexture(*ppTexture);
        *ppTexture = nullptr;
    }

    // Free the SDL renderer
    if (ppRenderer)
    {
        SDL_DestroyRenderer(*ppRenderer);
        *ppRenderer = nullptr;
    }

    // Free the SDL window
    if (ppWindow)
    {
        SDL_DestroyWindow(*ppWindow);
        *ppWindow = nullptr;
    }
}

// Initialize SDL Components 
int32_t Startup(SDL_Window** ppWindow, SDL_Renderer** ppRenderer, SDL_Texture** ppTexture)
{
    SDL_Init(SDL_INIT_VIDEO);

    if (!ppWindow)
    {
        cout << "Potiner to Window* was null\n";
        return -1;
    }
    *ppWindow = CreateCenteredWindow();

    if (!*ppWindow)
    {
        cout <<  "No Window. Aborting...\n";
        Shutdown(ppWindow, ppRenderer, ppTexture);

        return -1;
    }

    if (!ppRenderer)
    {
        cout << "Pointer to Renderer* was null\n";
        return -1;
    }

    *ppRenderer = CreateRenderer(*ppWindow, true);

    if (!ppRenderer )
    {
        cout << "No Renderer. Aborting...";
        Shutdown(ppWindow, ppRenderer, ppTexture);

        return -1;
    }

    if (!ppTexture)
    {
        cout << "Pointer to Texture* was null\n";
        return -1;
    }

    *ppTexture = CreateBackBufferTexture(*ppRenderer);

    if (!*ppTexture)
    {
        cout << "No back buffer Texture. Aborting...";
        Shutdown(ppWindow, ppRenderer, ppTexture);

        return -1;
    }

    return 0;
}


//draws the screen
void DrawScreen(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture)
{
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer); //clear the screen

    for (Chunk* chunk : theWorld.chunks) {
        for (size_t x = 0; x < chunk->width; x++)
            for (size_t y = 0; y < chunk->height; y++) {
                Cell& cell = chunk->GetCell(x + y * chunk->width);
                SDL_SetRenderDrawColor(pRenderer, cell.red, cell.green, cell.blue, cell.alpha);
                SDL_RenderDrawPoint(pRenderer, x + (chunk->x), y + (chunk->y));
            }
        DrawRect(pWindow, pRenderer, pTexture, chunk);
    }
    SDL_RenderPresent(pRenderer);
}

//draws the dirty rects
void DrawRect(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture, Chunk* chunk)
{
    SDL_Rect rect;
    rect.x = chunk->minX + chunk->x;
    rect.y = chunk->minY + chunk->y;
    rect.w = (chunk->maxX - chunk->minX);
    rect.h = (chunk->maxY - chunk->minY);
   
    SDL_SetRenderDrawColor(pRenderer, 144, 255, 144, 255);
    SDL_RenderDrawRect(pRenderer, &rect);
}




bool MoveDown(size_t x, size_t y, const Cell& cell)
{
    bool down = theWorld.IsEmpty(x, y + 1);
    if (down) {
        theWorld.MoveCell(x, y, x, y + 1);
    }

    return down;
}

bool MoveDownSide(size_t x, size_t y, const Cell& cell)
{
    bool downLeft = theWorld.IsEmpty(x - 1, y + 1);
    bool downRight = theWorld.IsEmpty(x + 1, y + 1);

    if (downLeft && downRight) {
        downLeft = (rand() % 2) > 0;
        downRight = !downLeft;
    }

    if (downLeft)  theWorld.MoveCell(x, y, x - 1, y + 1);
    else if (downRight) theWorld.MoveCell(x, y, x + 1, y + 1);

    return downLeft || downRight;
}


bool MoveSide(size_t x, size_t y, const Cell& cell)
{
    bool left = theWorld.IsEmpty(x - 1, y);
    bool right = theWorld.IsEmpty(x + 1, y);

    if (left && right) {
        left = (rand() % 2) > 0;
        right = !left;
    }

    if (left)  theWorld.MoveCell(x, y, x - 1, y);
    else if (right) theWorld.MoveCell(x, y, x + 1, y);

    return left || right;
}


//makes the sand fall
void RunSim()
{
    for (int i = 0; i < size(theWorld.chunks); i++) {
        Chunk* chunk = theWorld.chunks[i];
        for (size_t x = 0; x < chunk->width; x++)
            for (size_t y = 0; y < chunk->height; y++) {
                Cell& cell = chunk->GetCell(x + y*chunk->width);

                int px = x + (chunk->x);
                int py = y + (chunk->y);

                if (cell.Props & CellProperties::MOVE_DOWN && MoveDown(px, py, cell)) {}
                else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(px, py, cell)) {}
                else if (cell.Props & CellProperties::MOVE_SIDE && MoveSide(px, py, cell)) {}
            }
    }

    for (Chunk* chunk : theWorld.chunks) //commit changes
    {
        chunk->CommitCells();
    }

   
}



void mousePress(SDL_MouseButtonEvent& b, int xt, int yt)
{
    Cell placeMe;
    if (b.button == SDL_BUTTON_LEFT)
    {
        placeMe = _SAND;
    }
    else
    {
        placeMe = _WATER;
    }

    for (int x = xt; x < xt + 20; x++)
    {
        for (int y = yt; y < yt + 20; y++)
        {
            if (!theWorld.InBounds(x, y)) continue;

            theWorld.SetCell(x, y, placeMe);
        }
    }
    printf("Sandy\n");
}




// Call this once during each render loop in order to determine when the user wishes to terminate the program
bool ProcessInput()
{
    // Return this value to tell the caller whether or not it should continue rendering
    // We will terminate the application if any key is pressed
    bool keepRenderLoopRunning = true;

    // Events are generated by SDL whenever something occurs system-wide
    // We are only interested in keyboard events and when the user closes the window
    // We will terminate the application if a key is pressed or if the window is manually closed
    SDL_Event event;

    // Process all events and return whether or not to quit
    while (SDL_PollEvent(&event))
    {
        // Handle relevant SDL events
        switch (event.type)
        {
            // Terminate application if a key is pressed or if the user closes the window
        case SDL_KEYDOWN:
        case SDL_QUIT:
            keepRenderLoopRunning = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mousePress(event.button, event.button.x, event.button.y);
            break;
        }
    }

    // Let the caller know if it should continue rendering, otherwise terminate
    return keepRenderLoopRunning;
}



//sets up the global cells, the types of stuff that will be in the world
void InitializeCells()
{
    _EMPTY = {
        CellType::EMPTY,
        CellProperties::NONE,
        0, 0, 0, 0 // 0 alpha allows for a background
    };

    _SAND = {
        CellType::SAND,
        CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
        235, 200, 175
    };

    _WATER = {
        CellType::WATER,
        CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
        175, 200, 235
    };

    _ROCK = {
        CellType::ROCK,
        CellProperties::NONE,
        200, 200, 200
    };

    // Init a texture for sand...
}



//does update stuff, runs sim, draws window
void Update(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture)
{


    //RunSim();

    theWorld.RemoveEmptyChunks();

    for (int i = 0; i < size(theWorld.chunks); i++) {
        Chunk* chunk = theWorld.chunks[i];
        SimpleWorker(theWorld, chunk).UpdateChunk();
    }

    for (int i = 0; i < size(theWorld.chunks); i++) {
        Chunk* chunk = theWorld.chunks[i];
        chunk->CommitCells();
    }

    DrawScreen(pWindow, pRenderer, pTexture);

    return;
}




//plays the game, runs update, processess inputs, draws the screen
void PlayGame(SDL_Window* pWindow, SDL_Renderer* pRenderer, SDL_Texture* pTexture)
{
    bool running = true;

    while (running)
    {
        running = ProcessInput();
        Update(pWindow, pRenderer, pTexture);

    }
    return;
}


int main(int argc, char* args[])
{
    SDL_Window* pWindow = nullptr;
    SDL_Renderer* pRenderer = nullptr;
    SDL_Texture* pTexture = nullptr;



    InitializeCells(); //get the global cells ready


    //setup the window, return if it fails
    if (Startup(&pWindow, &pRenderer, &pTexture))
    {
        cout << "Startup Failed. Aborting...\n";
        Shutdown(&pWindow, &pRenderer, &pTexture);
        return -1;
    }

    Cell placeMe = _SAND;
    Cell meToo = _WATER;

    theWorld.SetCell(SCREEN_WIDTH / 2, 0, meToo);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 1, 0, placeMe);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 2, 0, placeMe);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 3, 0, placeMe);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 4, 0, placeMe);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 5, 0, meToo);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 6, 0, meToo);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 10, 0, meToo);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 50, 0, meToo);
    theWorld.SetCell(SCREEN_WIDTH / 2 + 54, 0, meToo);

    PlayGame(pWindow, pRenderer, pTexture);

    Shutdown(&pWindow, &pRenderer, &pTexture);
    return 0;
}
