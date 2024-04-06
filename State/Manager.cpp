//
// Created by Klavio Tarka on 13.12.23.
//
#include "Manager.h"

#include <memory>

#include "Race/Structure/Structure.h"
#include "Race/Unit/Unit.h"

Manager::Manager() {
  // game start
  map = Map();
  player = Player(map);
  enemy = Player(map);

  player.SetInitialCoordinates(Vec2(8, 2));
  enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 3));
  actionT t = enemy.units[0];
  player.units[0]->InsertAction(t);
  player.units[0]->TakeAction();

  // player.units[1]->MoveDij(Vec2(6, 7), *player.map->graph);
  MainLoop();
}

void Manager::MainLoop() {
  while ((player.HasUnit(PEASANT) && player.HasStructure(HALL)) &&
         (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
    for (int i = 0; i < player.units.size(); i++) {
      if (player.units[i]->GetActionQueueSize() > 0) {
        player.units[i]->TakeAction();
      }
      std::cout << player.units[0]->coordinate.x << " ";
      std::cout << player.units[0]->coordinate.y << "\n";
      //  player.RecruitSoldier(ARCHMAGE);
      //    std::cout << player.units.size() << " ";
      // std::cout << enemy.units[0]->health << "\n";
      // player.units[0]->Attack(*enemy.units[0]);
      // std::cout << enemy.units[0]->health;
    }
  }
}

void Manager::CheckForMovement() {}

float Manager::GetTime() {
  time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> diff = time - time1;
  return diff.count();
}

void Manager::ManageLiving(Player &pl) { std::cout << "dead"; }
