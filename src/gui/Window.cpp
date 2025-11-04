#include "Window.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Macro.h"
#include "../State/Map.h"
#include "../State/Player.h"

Window::Window(Vec2 s) : window_size(s) {
//    canvas_start = Vec2(win_start_x * 4, win_start_y * 4);
//    canvas_size_x = algo_start_x - win_start_x - canvas_start.x;
//    canvas_size_y = window_size.y - canvas_start.y;
//
//    map.text = "Map";
//    map.pos = Vec2(win_start_x, win_start_y);
//    dqn.text = "DQN";
//    dqn.pos = Vec2(algo_start_x, win_start_y);
//    ppo.text = "PPO";
//    ppo.pos = Vec2(algo_start_x, 200);
//    moves.text = "Moves";
//    moves.pos = Vec2(algo_start_x, 400);
//
//    SDL_AppResult result = InitSdl();


    canvas_start = Vec2(win_start_x * 4, win_start_y * 4);
    // Corrected: Removed the extra '- win_start_x' to use the full canvas width
    canvas_size_x = algo_start_x - canvas_start.x; 
    canvas_size_y = window_size.y - canvas_start.y;

    map.text = "Map";
    map.pos = Vec2(win_start_x, win_start_y);
    dqn.text = "DQN";
    dqn.pos = Vec2(algo_start_x, win_start_y);
    ppo.text = "PPO";
    ppo.pos = Vec2(algo_start_x, 200);
    moves.text = "Moves";
    moves.pos = Vec2(algo_start_x, 400);
    InitSdl();
}

SDL_AppResult Window::InitSdl() {
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

void Window::RenderPlayerUI(Player& pl, Player& en){
    
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
    SDL_FRect dst;

    TTF_Font* smallFont = TTF_OpenFont(fontPath, 13.0f);
    if (!smallFont) {
        SDL_Log("Failed to load small font: %s", SDL_GetError());
        return;
    }

    // --- Player 1 (DQN) ---
    std::string stats1 = "Gold: " + std::to_string(pl.gold) + " | Food: " + std::to_string(pl.food.x)+ "/" + std::to_string(pl.food.y);
    SDL_Surface* textSurface = TTF_RenderText_Blended(smallFont, stats1.c_str(), stats1.size(), color);
    if (textSurface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_DestroySurface(textSurface);
        if (texture) {
            dst.x = algo_start_x + 5;
            dst.y = 260;
            SDL_GetTextureSize(texture, &dst.w, &dst.h);
            dst.w /= 2;
            dst.h /= 2;
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
    }

    // --- Player 2 (PPO) ---
    std::string stats2 = "Gold: " + std::to_string(en.gold) + " | Food: " + std::to_string(en.food.x) + "/" + std::to_string(en.food.y);
    textSurface = TTF_RenderText_Blended(smallFont, stats2.c_str(), stats2.size(), color);
    if (textSurface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_DestroySurface(textSurface);
        if (texture) {
            dst.x = algo_start_x + 5;
            dst.y = 300;
            SDL_GetTextureSize(texture, &dst.w, &dst.h);
            dst.w /= 2;
            dst.h /= 2;
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
    }

    TTF_CloseFont(smallFont);
}

/**
 * @brief Gets the abbreviation for an object and renders it above the object's position.
 */

void Window::RenderObjectLabel(objectType& t, float gui_x, float gui_y) {
    std::string label;
    if (std::holds_alternative<UnitType>(t)) {
        switch(std::get<UnitType>(t)){
            case UnitType::PEASANT: label = "P"; break;
            case UnitType::FOOTMAN: label = "F"; break;
            default: label = "?"; break;
        }
    } else if (std::holds_alternative<StructureType>(t)) {
        switch(std::get<StructureType>(t)){
            case StructureType::HALL: label = "H"; break;
            case StructureType::BARRACK: label = "B"; break;
            case StructureType::FARM: label = "Fm"; break;
            default: label = "?"; break;
        }
    } else {
        label = "?";
    }

    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE}; 
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, label.c_str(), label.size(), color);

    if (!textSurface) {
        SDL_Log("Failed to create text surface: %s", SDL_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);

    if (!textTexture) {
        SDL_Log("Failed to create text texture: %s", SDL_GetError());
        return;
    }

    SDL_FRect textDst;
    SDL_GetTextureSize(textTexture, &textDst.w, &textDst.h);

    textDst.w /= 2.0f;
    textDst.h /= 2.0f;
    textDst.x = gui_x + (3.0f / 2.0f) - (textDst.w / 2.0f);
    textDst.y = gui_y - textDst.h - 2.0f; 

    SDL_RenderTexture(renderer, textTexture, NULL, &textDst);
    SDL_DestroyTexture(textTexture);
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

void Window::RenderMap(Player& pl, Player& en, Map& map) {
    // Using the previous fix for ratio calculation
  
    float ratiox = canvas_size_x / MAP_SIZE;
    float ratioy = canvas_size_y / MAP_SIZE;

    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (map.terrain[i][j].type == GOLD) {
                float gui_x = canvas_start.x + i * ratiox;
                float gui_y = canvas_start.y + j * ratioy;

                SDL_Vertex verts[3];
                // Define a small 3px wide, 3px high triangle
                verts[0].position = {gui_x, gui_y};
                verts[1].position = {gui_x + 3.0f, gui_y};
                verts[2].position = {gui_x + 1.5f, gui_y + 3.0f};

                // Use normalized float color (0–1)
                // --- CHANGED TO WHITE ---
                SDL_FColor whiteColor = {1.0f, 1.0f, 1.0f, 1.0f}; 
                for (int v = 0; v < 3; ++v)
                    verts[v].color = whiteColor; // Apply white color

                SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
            }
        }
    } 
    //
    SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);

    for (auto &obj : pl.units)
    {
        objectType ttype;
        ttype = obj->is;
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;
        
        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
        RenderObjectLabel(ttype, gui_x, gui_y); // Call new function
    }
    
    // --- Team 2 Units (game_objects2) ---
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);

    for (auto &obj : en.units)
    {
        objectType ttype;
        ttype = obj->is;
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
        RenderObjectLabel(ttype, gui_x, gui_y); // Call new function
    }

    // --- Team 1 Structures (game_objects3) ---
    SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);

    for (auto &obj : pl.structures)
    {
        objectType ttype;
        ttype = obj->is;
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
        RenderObjectLabel(ttype, gui_x, gui_y); // Call new function
    }
    // --- Team 2 Structures (game_objects4) ---
    //

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    for (auto &obj : en.structures)
    {
        objectType ttype;
        ttype = obj->is;
        float gui_x = canvas_start.x + obj->coordinate.x * ratiox;
        float gui_y = canvas_start.y + obj->coordinate.y * ratioy;

        SDL_FRect point = {gui_x, gui_y, 3.0f, 3.0f};
        SDL_RenderFillRect(renderer, &point);
        RenderObjectLabel(ttype, gui_x, gui_y); // Call new function
    }
}

SDL_AppResult Window::Render(Player& pl, Player& en, Map& map, std::string &dqn_action, std::string &ppo_action) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderUI();
    RenderMoves(dqn_action, ppo_action);
    RenderPlayerUI(pl, en);
    RenderMap(pl, en, map);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void Window::SDL_AppQuit() {
    if (font)
        TTF_CloseFont(font);
}
