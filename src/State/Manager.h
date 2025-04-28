#ifndef MANAGER_H
#define MANAGER_H

#include <chrono>
#include <memory>
#include <variant>

#include "../Living.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../ReinforcementLearning/RlManager.h"
#include "../ReinforcementLearning/Transition.h"
#include "../State/Map.h"

#include "Player.h"

class Manager
{
  public:
    Manager();
    void MainLoop();
    void ManageLiving(Player &pl);
    float GetTime();
    void CheckForMovement();
    bool Is10thSecond();
    void CheckForOwnership(Player &p, Living *, actionT);
    void PrintVector(Vec2 a);

  public:
    Player player;
    Player enemy;
    RlManager trainerManager;
    Map map;
    std::chrono::high_resolution_clock::time_point time1 =
        std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point time =
        std::chrono::high_resolution_clock::now();

  private:
    int frames = 0;

    duration<float, std::milli> frameCooldown = std::chrono::milliseconds(4000);
    high_resolution_clock::time_point frameTimer = high_resolution_clock::now();
};

#endif
