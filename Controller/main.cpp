#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

struct t_move {
    int16_t speed;
    int16_t rot;
};
struct t_radar {
    int16_t angle;
    int16_t dist;
};


SDL_Window* init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Window* window = SDL_CreateWindow("hello_sdl2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_RenderSetVSync(SDL_GetRenderer(window), 1);
    return window;
}

IPaddress initNetwork() {
    if (SDLNet_Init() == -1) {
        fprintf(stderr, "ER: SDLNet_Init: %sn", SDLNet_GetError());
        exit(-1);
    }
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, "192.168.10.1", 2300) != 0) {
        fprintf(stderr, "Erreur de resolution d'IP\n");
        exit(1);
    }
    return ip;
}

t_move move;

int main(int argc, char const* argv[]) {
    SDL_Window* window = init();
    IPaddress ip = initNetwork();
    move.speed = 0;
    move.rot = 0;
    bool quit = false;
    while (!quit) {
        // SDL_Renderer* renderer = SDL_GetRenderer(window);
        SDL_Surface* surface = SDL_GetWindowSurface(window);
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0x00, 0xFF));
        //SDL_RenderText

        SDL_UpdateWindowSurface(window);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: quit = true; break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_UP: move.speed = 255; break;
                case SDLK_DOWN: move.speed = -255; break;
                case SDLK_LEFT: move.rot = 255; break;
                case SDLK_RIGHT: move.rot = -255; break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: quit = true; break;
                case SDLK_UP:
                case SDLK_DOWN: move.speed = 0; break;
                case SDLK_LEFT:
                case SDLK_RIGHT: move.rot = 0; break;
                }
                break;
            }
        }
        TCPsocket serveur = SDLNet_TCP_Open(&ip);
        SDLNet_TCP_Accept(serveur);
        if (serveur == NULL) {
            fprintf(stderr, "Erreur de connexion au serveur : %s\n", SDLNet_GetError());
        } else {
            if (SDLNet_TCP_Send(serveur, &move, sizeof(move)) < sizeof(move)) {
                fprintf(stderr, "erreur d'envoie : %s\n", SDLNet_GetError());
            } else {
                t_radar radar;
                radar.angle = 0;
                radar.dist = 0;
                int r = SDLNet_TCP_Recv(serveur, &radar, sizeof(radar));
                fprintf(stdout, "send and recv %d %d\n", radar.angle, radar.dist);
                // int r = SDLNet_TCP_Recv(serveur, NULL, sizeof(NULL));
            }
            SDLNet_TCP_Close(serveur);
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}