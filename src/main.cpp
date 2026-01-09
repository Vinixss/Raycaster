#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *r_Window = NULL;
static SDL_Renderer *r_Renderer = NULL;

const int world[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

// **appstate is a place we can store a pointer for future state use
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("Ray3D", 640, 480, 0, &r_Window, &r_Renderer)) {
        return SDL_APP_FAILURE;
    }
    
    SDL_SetRenderLogicalPresentation(r_Renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX); //Not particularly needed as the window isn't resizable

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if(event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) 
{
    //SDL handles this apparently. Most likely needed later if I have anything else I need to destroy
}
