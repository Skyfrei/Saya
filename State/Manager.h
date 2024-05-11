#pragma once

#include <chrono>
#include <memory>
#include <variant>

#include "../Living.h"
#include "../Map/Map.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../ReinforcementLearning/RlManager.h"
#include "Player.h"

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction>;

class Manager {
 public:
  Manager();

  void MainLoop();
  void ManageLiving(Player &pl);
  float GetTime();
  void CheckForMovement();
  int GetFrames();
  void CheckForOwnership(Player &p, Living *, actionT);

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
  const int rlFrameCondition = 10;
  std::chrono::high_resolution_clock::time_point frameTimer =
      std::chrono::high_resolution_clock::now();
};
