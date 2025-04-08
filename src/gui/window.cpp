#include "window.h"

struct RenderStruct{
    RenderStruct(const char* txt, Vec2 p) : text(txt), pos(p){}
    const char* text;
    Vec2 pos;
};

Window::Window(Vec2 s) : window_size(s){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    SDL_AppResult result = InitSdl();
    
}

SDL_AppResult Window::InitSdl(){
     
    if (!SDL_CreateWindowAndRenderer(project_name, window_size.x, window_size.y, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;

}
SDL_AppResult Window::Render(){
    RenderStruct map("map", Vec2(0,0));
    RenderStruct advice("advice", Vec2(600, 0));
    RenderStruct moves("moves", Vec2(600, 600));

    SDL_SetRenderScale(renderer, scale, scale); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, map.pos.x, map.pos.y, map.text);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void Window::SDL_AppQuit(){

}

