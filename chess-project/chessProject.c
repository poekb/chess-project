#include "graphics/boardRenderer.h"
#include "moveGeneration/precompution.h"
#include "game/gameHandler.h"
#include "zobrist/zobristHashing.h"

int main(int argc, char* argv[]) {

    InitSDL(1080, 720, &window, &renderer);
    SDL_SetWindowMinimumSize(window, 480, 480);
    RendererInit(renderer);

    InitZobrist();
    PreCompute();
    
    // Start the main update loop
    UpdateLoop();
    
    RedererCleanup();
    SDL_Quit();

    return 0;
}
