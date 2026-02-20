#include "Window.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Macro.h"
#include "../State/Map.h"
#include "../State/Player.h"

Window::Window(Vec2 s) : window_size(s) {

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
    smallFont = TTF_OpenFont(fontPath, 13.0f); // Load small font ONCE here
    if (!font || !smallFont)
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

    // --- DQN TEXTURE CACHE ---
    if (dqn_action != cached_dqn_text) {
        if (dqn_tex) SDL_DestroyTexture(dqn_tex); // Free the old GPU resource
        
        SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, dqn_action.c_str(), 0, color, 250);
        if (surf) {
            dqn_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_DestroySurface(surf);
            cached_dqn_text = dqn_action; // Update the cache key
        }
    }

    if (dqn_tex) {
        dst.x = algo_start_x + 5;
        dst.y = win_start_y + 10;
        SDL_GetTextureSize(dqn_tex, &dst.w, &dst.h);
        dst.w /= 2.0f; dst.h /= 2.0f;
        SDL_RenderTexture(renderer, dqn_tex, NULL, &dst);
    }

    // --- PPO TEXTURE CACHE ---
    if (ppo_action != cached_ppo_text) {
        if (ppo_tex) SDL_DestroyTexture(ppo_tex); // Free the old GPU resource

        SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, ppo_action.c_str(), 0, color, 250);
        if (surf) {
            ppo_tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_DestroySurface(surf);
            cached_ppo_text = ppo_action; // Update the cache key
        }
    }

    if (ppo_tex) {
        dst.x = algo_start_x + 5;
        dst.y = 220; 
        SDL_GetTextureSize(ppo_tex, &dst.w, &dst.h);
        dst.w /= 2.0f; dst.h /= 2.0f;
        SDL_RenderTexture(renderer, ppo_tex, NULL, &dst);
    }
}

void Window::RenderPlayerUI(Player& pl, Player& en) {
    if (!smallFont || !renderer) return; 

    SDL_Color color = {255, 255, 255, 255};
    
    auto drawStatLine = [&](Player& p, float y, std::string label) {
        std::string s = label + "-> Gold: " + std::to_string(p.gold) + 
                        " | Food: " + std::to_string(p.food.x) + "/" + std::to_string(p.food.y);
        if (smallFont){
            SDL_Surface* surf = TTF_RenderText_Blended(smallFont, s.c_str(), 0, color);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                if (tex) {
                    SDL_FRect dst;
                    SDL_GetTextureSize(tex, &dst.w, &dst.h);
                    dst.x = algo_start_x + 5;
                    dst.y = y;
                    dst.w /= 2.0f; dst.h /= 2.0f;
                    SDL_RenderTexture(renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                }
                SDL_DestroySurface(surf);
            }
        }
    };

    drawStatLine(pl, 460.0f, "Pl");
    drawStatLine(en, 500.0f, "En");
}


void Window::RenderObjectLabel(objectType& t, float gui_x, float gui_y) {
    std::string label;
    if (std::holds_alternative<UnitType>(t)) {
        switch(std::get<UnitType>(t)){
            case UnitType::PEASANT: label = "P"; break;
            case UnitType::FOOTMAN: label = "S"; break;
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


void Window::PickColor(objectType& t, int p) {
    if (p == 0) {
        // Player Team: Bright Yellow
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    } else {
        // Enemy Team: Bright Red
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    }
}

void Window::RenderMap(Player& pl, Player& en, Map& map) {
    // Using the previous fix for ratio calculation
  
    float ratiox = canvas_size_x / MAP_SIZE;
    float ratioy = canvas_size_y / MAP_SIZE;

    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (map.terrain[i][j].type == GOLD) {
                float gui_x = canvas_start.x + j * ratiox;
                float gui_y = canvas_start.y + i * ratioy;

                SDL_Vertex verts[3];

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
// --- Helper to draw dots and labels ---
    auto drawDots = [&](auto& list, int pID) {
        for (auto &obj : list) {
            objectType currentObjType = obj->is; 
            float gui_x = canvas_start.x + obj->coordinate.y * ratiox;
            float gui_y = canvas_start.y + obj->coordinate.x * ratioy;
            // 3. Center the dot in the tile
            float dotSize = 4.0f;
            float final_dot_x = gui_x + (ratiox - dotSize) / 2.0f;
            float final_dot_y = gui_y + (ratioy - dotSize) / 2.0f;

            PickColor(currentObjType, pID);
            
            SDL_FRect dot = { final_dot_x, final_dot_y, dotSize, dotSize };
            SDL_RenderFillRect(renderer, &dot);

            // Pass the ACTUAL dot position, not the tile corner
            RenderObjectLabel(currentObjType, final_dot_x, final_dot_y);
        }
    };

    // --- Draw everything ---
    drawDots(pl.units, 0);       // Team 0 (Yellow)
    drawDots(en.units, 1);       // Team 1 (Green/Purple)
    drawDots(pl.structures, 0);
    drawDots(en.structures, 1);
}

SDL_AppResult Window::Render(Player& pl, Player& en, Map& map, std::string dqn_action, std::string ppo_action) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return SDL_APP_SUCCESS; // Signal to stop
        }
    }
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
    if (smallFont)
        TTF_CloseFont(smallFont);
    if (dqn_tex)
        SDL_DestroyTexture(dqn_tex);
    if (ppo_tex)
        SDL_DestroyTexture(ppo_tex);


}
