#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *r_Window = NULL;
static SDL_Renderer *r_Renderer = NULL;

int player_x = 256;
int player_y = 256;

#define CELL_SIZE 64
#define PLAYER_SIZE 8
#define WORLD_SIZE 8

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

void set_player_pos(SDL_FRect * p, int x, int y) {
    p->x = (float) x;
    p->y = (float) y;
}

void player_move(int d_x, int d_y) {
    int x_ind_t = (player_x + d_x) / CELL_SIZE;
    int x_ind_b = (player_x + d_x + PLAYER_SIZE) / CELL_SIZE;
    int y_ind_t = (player_y + d_y) / CELL_SIZE;
    int y_ind_b = (player_y + d_y + PLAYER_SIZE) / CELL_SIZE;

    if(world[x_ind_t + y_ind_t * 8] == 0 &&
        world[x_ind_b + y_ind_t * 8] == 0 &&
        world[x_ind_t + y_ind_b * 8] == 0 &&
        world[x_ind_b + y_ind_b * 8] == 0)
    {
        player_x += d_x;
        player_y += d_y;
    }
}

SDL_AppResult handle_key_press(SDL_Keycode key_code) {
    switch (key_code) {
        case SDLK_W:
            player_move(0, -8);
            break;
        case SDLK_A:
            player_move(-8, 0);
            break;
        case SDLK_S:
            player_move(0, 8);
            break;
        case SDLK_D:
            player_move(8, 0);
            break;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

// **appstate is a place we can store a pointer for future state use
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("Ray3D", WORLD_SIZE * CELL_SIZE, WORLD_SIZE * CELL_SIZE, 0, &r_Window, &r_Renderer)) {
        return SDL_APP_FAILURE;
    }
    
    SDL_SetRenderLogicalPresentation(r_Renderer, WORLD_SIZE * CELL_SIZE, WORLD_SIZE * CELL_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX); //Not particularly needed as the window isn't resizable

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(r_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(r_Renderer);

    SDL_FRect r;
    r.w = r.h = CELL_SIZE - 1;
    SDL_FRect player_r;
    player_r.w = player_r.h = PLAYER_SIZE;

    SDL_SetRenderDrawColor(r_Renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    set_player_pos(&player_r, player_x, player_y);
    SDL_RenderFillRect(r_Renderer, &player_r);

    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int j = 0; j < WORLD_SIZE; j++) {
            r.x = (float) i * CELL_SIZE;
            r.y = (float) j * CELL_SIZE;
            if(world[i + WORLD_SIZE * j] == 1) {
                SDL_SetRenderDrawColor(r_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            }
            else {
                continue;
            }
            SDL_RenderFillRect(r_Renderer, &r);
        }
    }
    
    SDL_RenderPresent(r_Renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch(event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            return handle_key_press(event->key.key);
        default:
            break;

    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) 
{
    //SDL handles this apparently. Most likely needed later if I have anything else I need to destroy
}
