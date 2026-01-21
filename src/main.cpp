#define SDL_MAIN_USE_CALLBACKS
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define CELL_SIZE 64
#define PLAYER_SIZE 8
#define WORLD_SIZE 8
#define SPEED 80

//Perhaps put the rectange in here to get rid of some of these redundant variables
typedef struct {
    float pos[2];
    float vel[2];
    int world_pos[4];
    int old_world_pos[4];
} Player;


typedef struct {
    Player player;
    SDL_Window *r_Window;
    SDL_Renderer *r_Renderer;
} AppState;


const bool * keys = SDL_GetKeyboardState(nullptr);
uint64_t prevTime = SDL_GetTicks();


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


void set_player_pos(SDL_FRect * p, Player * player) {
    p->x = player->pos[0];
    p->y = player->pos[1];
}


void init_player(Player * p) {
    p->pos[0] = 255.0;
    p->pos[1] = 255.0;
    p->vel[0] = p->vel[1] = 0.0;
    p->world_pos[0] = p->old_world_pos[0] = (int) p->pos[0] / CELL_SIZE;
    p->world_pos[1] = p->old_world_pos[1] = (int) p->pos[1] / CELL_SIZE;
    p->world_pos[2] = p->old_world_pos[2] = (int) (p->pos[0] + PLAYER_SIZE) / CELL_SIZE;
    p->world_pos[3] = p->old_world_pos[3] = (int) (p->pos[1] + PLAYER_SIZE) / CELL_SIZE;
}


void update_player(Player * p, float time) {
    //First update based on what has been pressed

    p->old_world_pos[0] = p->world_pos[0];
    p->old_world_pos[1] = p->world_pos[1];
    p->old_world_pos[2] = p->world_pos[2];
    p->old_world_pos[3] = p->world_pos[3];

    if(keys[SDL_SCANCODE_W]) {
        p->vel[1] += SPEED * -1;
    }
    if(keys[SDL_SCANCODE_A]) {
        p->vel[0] += SPEED * -1;
    }
    if(keys[SDL_SCANCODE_S]) {
        p->vel[1] += SPEED;
    }
    if(keys[SDL_SCANCODE_D]) {
        p->vel[0] += SPEED;
    }

    p->pos[0] += p->vel[0] * time;
    p->pos[1] += p->vel[1] * time;

    p->world_pos[0] = (int) p->pos[0] / CELL_SIZE;
    p->world_pos[1] = (int) p->pos[1] / CELL_SIZE;
    p->world_pos[2] = (int) (p->pos[0] + PLAYER_SIZE) / CELL_SIZE;
    p->world_pos[3] = (int) (p->pos[1] + PLAYER_SIZE) / CELL_SIZE;

    //Check collision and push the player out if need be
    if(p->vel[0] < 0 && world[p->world_pos[0] + p->old_world_pos[1] * 8] == 1 ) {
        p->world_pos[0] += 1;
        p->pos[0] = p->world_pos[0] * CELL_SIZE;
    }
    else if(p->vel[0] > 0 && world[p->world_pos[2] + p->old_world_pos[1] * 8] == 1) {
        p->pos[0] = p->world_pos[2] * CELL_SIZE - PLAYER_SIZE;
        p->world_pos[0] -= 1;
    }

    if(p->vel[1] < 0 && world[p->old_world_pos[0] + p->world_pos[1] * 8] == 1 ) {
        p->world_pos[1] += 1;
        p->pos[1] = p->world_pos[1] * CELL_SIZE;
    }
    else if(p->vel[1] > 0 && world[p->old_world_pos[3] + p->world_pos[1] * 8] == 1) {
        p->pos[1] = p->world_pos[3] * CELL_SIZE - PLAYER_SIZE;
        p->world_pos[1] -= 1;
    }

    p->vel[0] = p->vel[1] = 0;
}


// **appstate is a place we can store a pointer for future state use
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    AppState *state = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!state) {
        return SDL_APP_FAILURE;
    } 
    else {
        *appstate = state;
    }

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("Ray3D", WORLD_SIZE * CELL_SIZE, WORLD_SIZE * CELL_SIZE, 0, &(state->r_Window), &(state->r_Renderer))) {
        return SDL_APP_FAILURE;
    }
    
    init_player(&(state->player));

    SDL_SetRenderLogicalPresentation(state->r_Renderer, WORLD_SIZE * CELL_SIZE, WORLD_SIZE * CELL_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX); //Not particularly needed as the window isn't resizable

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate) {
    uint64_t curTime = SDL_GetTicks();

    AppState* state = (AppState*) appstate;
    SDL_FRect r;
    r.w = r.h = CELL_SIZE - 1;
    SDL_FRect player_r;
    player_r.w = player_r.h = PLAYER_SIZE;

    SDL_SetRenderDrawColor(state->r_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->r_Renderer);
    SDL_SetRenderDrawColor(state->r_Renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    float delta = (curTime - prevTime) / 1000.0f;
    update_player(&(state->player), delta);
    set_player_pos(&player_r, &(state->player));

    SDL_RenderFillRect(state->r_Renderer, &player_r);

    for(int i = 0; i < WORLD_SIZE; i++) {
        for(int j = 0; j < WORLD_SIZE; j++) {
            r.x = (float) i * CELL_SIZE;
            r.y = (float) j * CELL_SIZE;
            if(world[i + WORLD_SIZE * j] == 1) {
                SDL_SetRenderDrawColor(state->r_Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            }
            else {
                continue;
            }
            SDL_RenderFillRect(state->r_Renderer, &r);
        }
    }
    printf("Player is at (%.1f, %.1f) and cell (%d, %d)\n", state->player.pos[0], state->player.pos[1], state->player.world_pos[0], state->player.world_pos[1]);
    SDL_RenderPresent(state->r_Renderer);
    prevTime = curTime;
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch(event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void *appstate, SDL_AppResult result) 
{
    AppState * state = (AppState*) appstate;
    SDL_DestroyWindow(state->r_Window);
    SDL_DestroyRenderer(state->r_Renderer);
    SDL_free(state);
    //SDL handles this apparently. Most likely needed later if I have anything else I need to destroy
}
