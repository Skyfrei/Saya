#ifndef WINDOW_H
#define WINDOW_H
#include <SDL3/SDL.h>
#include "../Tools/Vec2.h"


class Window{
    public:
        Window(Vec2 s);
        SDL_AppResult Render();
        void SDL_AppQuit();

       
    private:
        SDL_AppResult InitSdl(); 
    private:
        Vec2 window_size;
        const float scale = 1.0f;
        const char* project_name = "Saya";
        SDL_Window* window;
        SDL_Renderer* renderer;
                
};


#endif
