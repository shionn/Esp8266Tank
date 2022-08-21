#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define MIN_RADAR_ANGLE 40
#define MAX_RADAR_ANGLE 140

struct t_move {
    int16_t speed;
    int16_t rot;
};
struct t_radar {
    int16_t angle;
    int16_t dist;
};


SDL_Window* init() {
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0) {
        fprintf( stderr, "could not initialize sdl2: %s\n", SDL_GetError() );
        exit( 1 );
    }
    SDL_Window* window = SDL_CreateWindow( "Tank controller", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (window == NULL) {
        fprintf( stderr, "could not create window: %s\n", SDL_GetError() );
        exit( 1 );
    }
    return window;
}

IPaddress initNetwork() {
    if (SDLNet_Init() == -1) {
        fprintf( stderr, "ER: SDLNet_Init: %sn", SDLNet_GetError() );
        exit( -1 );
    }
    IPaddress ip;
    if (SDLNet_ResolveHost( &ip, "192.168.10.1", 2300 ) != 0) {
        fprintf( stderr, "Erreur de resolution d'IP\n" );
        exit( 1 );
    }
    return ip;
}

t_move move;
bool quit = false;
t_radar radars[MAX_RADAR_ANGLE - MIN_RADAR_ANGLE + 1];
long maxRadar = 1;

void updateKeyboard() {
    SDL_Event event;
    while (SDL_PollEvent( &event )) {
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
}

void updateNetwork( IPaddress ip ) {
    TCPsocket serveur = SDLNet_TCP_Open( &ip );
    SDLNet_TCP_Accept( serveur );
    if (serveur == NULL) {
        fprintf( stderr, "Erreur de connexion au serveur : %s\n", SDLNet_GetError() );
    } else {
        if (SDLNet_TCP_Send( serveur, &move, sizeof( move ) ) < sizeof( move )) {
            fprintf( stderr, "erreur d'envoie : %s\n", SDLNet_GetError() );
        } else {
            t_radar radar;
            radar.angle = 0;
            radar.dist = 0;
            int r = SDLNet_TCP_Recv( serveur, &radar, sizeof( radar ) );
            if (r == sizeof( radar )) {
                fprintf( stdout, "send and recv %d %d\n", radar.angle, radar.dist );
                radars[radar.angle - MIN_RADAR_ANGLE] = radar;
            } else fprintf( stdout, "size %d %d \n", r, sizeof( radar ) );
            // int r = SDLNet_TCP_Recv(serveur, NULL, sizeof(NULL));
        }
        SDLNet_TCP_Close( serveur );
    }
}

int main( int argc, char const* argv[] ) {
    SDL_Window* window = init();
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, 0 );
    IPaddress ip = initNetwork();
    move.speed = 0;
    move.rot = 0;
    while (!quit) {
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer );
        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            int angle = x * (MAX_RADAR_ANGLE - MIN_RADAR_ANGLE) / SCREEN_WIDTH;
            maxRadar = radars[angle].dist > maxRadar ? radars[angle].dist : maxRadar;
            // maxRadar = 8000;
            int dist = radars[angle].dist > maxRadar ? maxRadar : radars[angle].dist;
            int  h = 400 * (maxRadar - dist) / maxRadar;
            if (h > 400) {
                fprintf( stdout, "%d %d %d %d \n", x, angle, maxRadar, h );
            }
            SDL_RenderDrawLine( renderer, SCREEN_WIDTH - x - 1, 400 - h, SCREEN_WIDTH - x - 1, 400 + h );
        }
        SDL_RenderPresent( renderer );
        updateKeyboard();
        updateNetwork( ip );
    }
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}