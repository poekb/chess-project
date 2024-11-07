#include "graphics/boardRenderer.h"
#include "moveGeneration/precompution.h"
#include "game/gameHandler.h"
#include "zobrist/zobristHashing.h"

int main(int argc, char* argv[]) {

    sdlInit(1000, 720, &window, &renderer);
    SDL_SetWindowMinimumSize(window, 480, 480);
    rendererInit(renderer);

    initZobrist();
    preCompute();
    
    // Start the main update loop
    updateLoop();
    
    redererCleanup();
    SDL_Quit();

    return 0;
}
