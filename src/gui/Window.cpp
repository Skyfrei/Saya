#include "Window.h"
#include "../Tools/Macro.h"
#include <iostream>
#include "../Race/Unit/Unit.h"

struct RenderStruct{
    RenderStruct(const char* txt, Vec2 p) : text(txt), pos(p){}
    const char* text;
    Vec2 pos;
};

Window::Window(Vec2 s) : window_size(s){
    canvas_start = Vec2(win_start_x * 4, win_start_y * 4);
    canvas_size_x = algo_start_x - win_start_x - canvas_start.x;
    canvas_size_y = window_size.y - canvas_start.y;
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
    RenderStruct map("Map", Vec2(win_start_x, win_start_y));
    RenderStruct dqn("DQN", Vec2(algo_start_x, win_start_y));
    RenderStruct ppo("PPO", Vec2(algo_start_x, 200));
    RenderStruct moves("Moves", Vec2(algo_start_x, 400));
                                                                                                            
    SDL_SetRenderScale(renderer, scale, scale); 
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, map.pos.x, map.pos.y, map.text);
    SDL_RenderDebugText(renderer, dqn.pos.x, dqn.pos.y, dqn.text);
    SDL_RenderDebugText(renderer, ppo.pos.x, ppo.pos.y, ppo.text);
    SDL_RenderDebugText(renderer, moves.pos.x, moves.pos.y, moves.text);
    const SDL_FRect parallel_line = {ppo.pos.x * 1.0f, moves.pos.y  - 10.0f, 1.0f * window_size.y - ppo.pos.y, 1};
    const SDL_FRect parallel_line2 = {dqn.pos.x * 1.0f, ppo.pos.y  - 10.0f, 1.0f * window_size.y - dqn.pos.y, 1};
    const SDL_FRect vertical_line = {dqn.pos.x * 1.0f, 0.0f, 1, window_size.y * 1.0f};
    SDL_RenderFillRect(renderer, &parallel_line);
    SDL_RenderFillRect(renderer, &parallel_line2);
    SDL_RenderFillRect(renderer, &vertical_line);
}

void Window::RenderMap(std::vector<Unit*>& game_objects, std::vector<Unit*>& game_objects2){
    float ratiox = canvas_size_x / MAP_SIZE;
    float ratioy = canvas_size_y / MAP_SIZE;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (auto& obj : game_objects){
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto& obj : game_objects2){
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }
}

SDL_AppResult Window::Render(std::vector<Unit*>& game_objects, std::vector<Unit*>& game_objects2){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderUI();
    RenderMap(game_objects, game_objects2);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void Window::SDL_AppQuit(){

}

