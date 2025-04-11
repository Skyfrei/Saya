#ifndef WINDOW_H
#define WINDOW_H
#include <SDL3/SDL.h>
#include "../Tools/Vec2.h"
#include <vector>

class Unit;

class Window{
    public:
        Window(Vec2 s);
        SDL_AppResult Render(std::vector<Unit*>& game_objects, std::vector<Unit*>& game_objects2);
        void SDL_AppQuit();
    private:
        SDL_AppResult InitSdl(); 
        void RenderUI();
        void RenderMap(std::vector<Unit*>& game_objects, std::vector<Unit*>& game_objects2);
        
    private:
        Vec2 window_size;
        const float scale = 1.5f;
        const char* project_name = "Saya";
        SDL_Window* window;
        SDL_Renderer* renderer;
        int win_start_x = 5;
        int win_start_y = 5;
        int algo_start_x = 550;
                
};


#endif
