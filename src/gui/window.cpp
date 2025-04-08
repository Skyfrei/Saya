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
void Window::RenderUI(){
    RenderStruct map("Map", Vec2(5,5));
    RenderStruct dqn("DQN", Vec2(550, 5));
    RenderStruct ppo("PPO", Vec2(550, 200));
    RenderStruct moves("Moves", Vec2(550, 400));
                                                                                                            
    SDL_SetRenderScale(renderer, scale * 1.5, scale * 1.5); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, map.pos.x, map.pos.y, map.text);
    SDL_RenderDebugText(renderer, dqn.pos.x, dqn.pos.y, dqn.text);
    SDL_RenderDebugText(renderer, ppo.pos.x, ppo.pos.y, ppo.text);
    SDL_RenderDebugText(renderer, moves.pos.x, moves.pos.y, moves.text);
    const SDL_FRect parallel_line = {ppo.pos.x * 1.0f, moves.pos.y  - 10.0f, window_size.y - ppo.pos.y, 1};
    const SDL_FRect parallel_line2 = {dqn.pos.x * 1.0f, ppo.pos.y  - 10.0f, window_size.y - dqn.pos.y, 1};
    const SDL_FRect vertical_line = {dqn.pos.x * 1.0f, 0.0f, 1, window_size.y * 1.0f};
    SDL_RenderFillRect(renderer, &parallel_line);
    SDL_RenderFillRect(renderer, &parallel_line2);
    SDL_RenderFillRect(renderer, &vertical_line);
    SDL_RenderPresent(renderer);
}

SDL_AppResult Window::Render(){

    RenderUI();
    return SDL_APP_CONTINUE;
}

void Window::SDL_AppQuit(){

}

