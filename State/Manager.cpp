//
// Created by Klavio Tarka on 13.12.23.
//
#include "Manager.h"

#include <memory>

#include "Race/Structure/Structure.h"
#include "Race/Unit/Unit.h"

Manager::Manager() : player(map), enemy(map) {
  // game start

  player.SetInitialCoordinates(Vec2(8, 2));
  enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));

  player.Move(player.units[0].get(), Vec2(13, 12));

  MainLoop();
}

void Manager::CheckForOwnership(Living *l, Vec2 v) {
  if (l->health <= 0) {
    map.RemoveOwnership(l, v);
  }
  if (l->coordinate.x != v.x && l->coordinate.y != v.y) {
    map.RemoveOwnership(l, v);
    map.AddOwnership(l);
  }
}

void Manager::MainLoop() {
  while ((player.HasUnit(PEASANT) && player.HasStructure(HALL)) &&
         (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
    for (int i = 0; i < player.units.size(); i++) {
      if (player.units[i]->GetActionQueueSize() > 0) {
        Vec2 temp = player.units[0]->coordinate;
        player.units[0]->TakeAction();
        CheckForOwnership(player.units[0].get(), temp);
      }
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
