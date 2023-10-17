#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_net.h"

int main(int argc, char** argv)
{
    // TODO: check return values
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(0);
    TTF_Init();
    SDLNet_Init();

    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}