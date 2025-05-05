#ifndef WINDOW_H
#define WINDOW_H
#include "../Tools/Vec2.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

class Unit;
// Just decouple this into sending only strings, but keep for testingv purposes
// rn
struct RenderStruct
{
    RenderStruct()
    {
    }
    const char *text;
    Vec2 pos;
};

class Window
{
  public:
    Window(Vec2 s);
    SDL_AppResult Render(std::vector<Unit *> &game_objects,
                         std::vector<Unit *> &game_objects2, std::string &dqn_Action,
                         std::string &ppo_action);
    void SDL_AppQuit();

  private:
    SDL_AppResult InitSdl();
    void RenderUI();
    void RenderMap(std::vector<Unit *> &game_objects,
                   std::vector<Unit *> &game_objects2);
    void RenderMoves(std::string &dqn_action, std::string &ppo_action);

  private:
    RenderStruct map;
    RenderStruct dqn;
    RenderStruct ppo;
    RenderStruct moves;
    TTF_Font *font;
    const char *fontPath = "./MonaspaceXenonFrozen-Regular.ttf";
    SDL_Surface *text;
    SDL_Texture *texture;

    Vec2 window_size;
    const float scale = 1.5f;
    const char *project_name = "Saya";
    SDL_Window *window;
    SDL_Renderer *renderer;
    int win_start_x = 5;
    int win_start_y = 5;
    int algo_start_x = 550;
    Vec2 canvas_start;
    int canvas_size_x;
    int canvas_size_y;
};

#endif
