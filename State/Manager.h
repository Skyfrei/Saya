#pragma once

#include <chrono>
#include <memory>
#include <variant>

#include "../Living.h"
#include "../Map/Map.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../ReinforcementLearning/ReplayMemory.h"
#include "Player.h"

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction, RecruitSoldierAction>;

class Manager {
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
  ReplayMemory trainerManager;
  Map map;
  std::chrono::high_resolution_clock::time_point time1 =
      std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point time =
      std::chrono::high_resolution_clock::now();

 private:
  int frames = 0;
  std::chrono::high_resolution_clock::time_point frameTimer =
      std::chrono::high_resolution_clock::now();
};
