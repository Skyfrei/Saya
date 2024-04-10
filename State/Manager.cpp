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
  enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));

  player.Move(player.units[0].get(), Vec2(13, 12));

  MainLoop();
}

void Manager::CheckForOwnership(Living *oldL, Living *newL) {
  if (oldL->coordinate != newL->coordinate) {
    map.RemoveOwnership(oldL);
    map.AddOwnership(newL, newL->coordinate);
  } else if (oldL->health <= 0) {
    map.RemoveOwnership(oldL);
  }
}

void Manager::MainLoop() {
  while ((player.HasUnit(PEASANT) && player.HasStructure(HALL)) &&
         (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
    for (int i = 0; i < player.units.size(); i++) {
      if (player.units[i]->GetActionQueueSize() > 0) {
        Living *temp = player.units[i].get();
        player.units[i]->TakeAction();
        CheckForOwnership(temp, player.units[i].get());
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
