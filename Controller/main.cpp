#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800


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
    if (SDLNet_ResolveHost(&ip, "192.168.50.200", 2300) != 0) {
        fprintf(stderr, "Erreur de resolution d'IP\n");
        exit(1);
    }
    return ip;
}



int main(int argc, char const* argv[]) {
    SDL_Window* window = init();
    IPaddress ip = initNetwork();
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
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: quit = true; break;
                case SDLK_s:
                    char* buf = "{s:256,r:0}";
                    TCPsocket serveur = SDLNet_TCP_Open(&ip);
                    if (serveur == NULL) {
                        fprintf(stderr, "Erreur de connexion au serveur : %s\n", SDLNet_GetError());
                    } else {
                        if (SDLNet_TCP_Send(serveur, buf, sizeof(buf)) < sizeof(buf)) {
                            fprintf(stderr, "erreur d'envoie : %s\n", SDLNet_GetError());
                        } else {
                            int r = SDLNet_TCP_Recv(serveur, NULL, sizeof(NULL));
                            fprintf(stdout, "send and recv %d\n", r);
                        }
                        SDLNet_TCP_Close(serveur);
                    }
                    break;
                }

            }
        }

    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}