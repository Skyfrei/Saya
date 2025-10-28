#include "Window.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Macro.h"

Window::Window(Vec2 s) : window_size(s) {
    canvas_start = Vec2(win_start_x * 4, win_start_y * 4);
    canvas_size_x = algo_start_x - win_start_x - canvas_start.x;
    canvas_size_y = window_size.y - canvas_start.y;

    map.text = "Map";
    map.pos = Vec2(win_start_x, win_start_y);
    dqn.text = "DQN";
    dqn.pos = Vec2(algo_start_x, win_start_y);
    ppo.text = "PPO";
    ppo.pos = Vec2(algo_start_x, 200);
    moves.text = "Moves";
    moves.pos = Vec2(algo_start_x, 400);

    SDL_AppResult result = InitSdl();
}

SDL_AppResult Window::InitSdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(project_name, window_size.x, window_size.y,
                                     SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_IOStream *stream = SDL_IOFromFile(fontPath, "rb");
    if (!stream)
    {
        SDL_Log("Failed to open font file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    font = TTF_OpenFontIO(stream, true, 22.0f);
    if (!font)
    {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}
void Window::RenderUI() {

    SDL_SetRenderScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, map.pos.x, map.pos.y, map.text);
    SDL_RenderDebugText(renderer, dqn.pos.x, dqn.pos.y, dqn.text);
    SDL_RenderDebugText(renderer, ppo.pos.x, ppo.pos.y, ppo.text);
    SDL_RenderDebugText(renderer, moves.pos.x, moves.pos.y, moves.text);
    const SDL_FRect parallel_line = {ppo.pos.x * 1.0f, moves.pos.y - 10.0f,
                                     1.0f * window_size.y - ppo.pos.y, 1};
    const SDL_FRect parallel_line2 = {dqn.pos.x * 1.0f, ppo.pos.y - 10.0f,
                                      1.0f * window_size.y - dqn.pos.y, 1};
    const SDL_FRect vertical_line = {dqn.pos.x * 1.0f, 0.0f, 1,
                                     window_size.y * 1.0f};
    SDL_RenderFillRect(renderer, &parallel_line);
    SDL_RenderFillRect(renderer, &parallel_line2);
    SDL_RenderFillRect(renderer, &vertical_line);
}

void Window::RenderMoves(std::string &dqn_action, std::string &ppo_action) {
    SDL_FRect dst;
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
    text = TTF_RenderText_Blended_Wrapped(font, dqn_action.data(),
                                          dqn_action.length(), color, 250);
    if (text)
    {
        texture = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!texture)
    {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
    }
    dst.x = algo_start_x + 5;
    dst.y = win_start_y + 10;
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.h /= 2;
    dst.w /= 2;
    SDL_RenderTexture(renderer, texture, NULL, &dst);

    text = TTF_RenderText_Blended_Wrapped(font, dqn_action.data(),
                                          dqn_action.length(), color, 250);
    if (text)
    {
        texture = SDL_CreateTextureFromSurface(renderer, text);
        SDL_DestroySurface(text);
    }
    if (!texture)
    {
        SDL_Log("Couldn't create text: %s\n", SDL_GetError());
    }
    dst.x = algo_start_x + 5;
    dst.y = 220;
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.h /= 2;
    dst.w /= 2;
    SDL_RenderTexture(renderer, texture, NULL, &dst);
}

void Window::PickColor(objectType& t, int p){
    if (std::holds_alternative<UnitType>(t)) {
        auto type = std::get<UnitType>(t);
        switch(type){
            case UnitType::PEASANT:
                if (p == 0)
                    SDL_SetRenderDrawColor(renderer, 255, 99, 71, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 60, 179, 113, 255);
                break;
            case UnitType::FOOTMAN:
                if (p == 0)
                    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 238, 130, 238, 255);
                break;
        }
    } else if (std::holds_alternative<StructureType>(t)) {
        auto type = std::get<StructureType>(t);
        switch(type){
            case StructureType::HALL:
                if (p == 0)
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 123, 104, 238, 255);
                break;
            case StructureType::BARRACK:
                if (p == 0)
                    SDL_SetRenderDrawColor(renderer, 244, 164, 96, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255);
                break;
            case StructureType::FARM:
                if (p == 0)
                    SDL_SetRenderDrawColor(renderer, 199, 21, 133, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
                break;
        }
    }
}

void Window::RenderMap(std::vector<Unit *> &game_objects,
                       std::vector<Unit *> &game_objects2,
                       std::vector<Structure *> &game_objects3,
                       std::vector<Structure *> &game_objects4) {
    float ratiox = canvas_size_x / MAP_SIZE;
    float ratioy = canvas_size_y / MAP_SIZE;
    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (auto &obj : game_objects)
    {
        objectType ttype;
        ttype = obj->is;
        PickColor(ttype, 0);
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        // std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;
        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto &obj : game_objects2)
    {
        objectType ttype;
        ttype = obj->is;
        PickColor(ttype, 1);
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        // std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }
    for (auto &obj : game_objects3)
    {
        objectType ttype;
        ttype = obj->is;
        PickColor(ttype, 1);
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        // std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }

    for (auto &obj : game_objects4)
    {
        objectType ttype;
        ttype = obj->is;
        PickColor(ttype, 1);
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        // std::cout<<obj->coordinate.x << " " << obj->coordinate.y<<std::endl;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
    }
}

SDL_AppResult Window::Render(std::vector<Unit *> &game_objects,
                             std::vector<Unit *> &game_objects2,
                             std::vector<Structure *> &game_objects3,
                             std::vector<Structure *> &game_objects4,
                             std::string &dqn_action, std::string &ppo_action) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderUI();
    RenderMoves(dqn_action, ppo_action);
    RenderMap(game_objects, game_objects2, game_objects3, game_objects4);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void Window::SDL_AppQuit() {
    if (font)
        TTF_CloseFont(font);
}
