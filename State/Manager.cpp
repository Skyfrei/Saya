//
// Created by Klavio Tarka on 13.12.23.
//
#include "Manager.h"

#include <memory>
#include <variant>

#include "Race/Structure/Structure.h"
#include "Race/Unit/Peasant.h"
#include "Race/Unit/Unit.h"

Manager::Manager() : player(map), enemy(map) {
  // game start

  player.SetInitialCoordinates(Vec2(8, 2));
  enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));

  player.Attack(player.units[0].get(), enemy.units[0].get());

  MainLoop();
}

void Manager::CheckForOwnership(Living *l, actionT actionTaken) {
  if (std::holds_alternative<AttackAction>(actionTaken)) {
    AttackAction &action = std::get<AttackAction>(actionTaken);
    if (action.object->health <= 0) {
      map.RemoveOwnership(action.object, action.object->coordinate);
    }
    if (l->coordinate.x != action.prevCoord.x ||
        l->coordinate.y != action.prevCoord.y) {
      map.RemoveOwnership(l, action.prevCoord);
      map.AddOwnership(l);
    }
  }
  if (std::holds_alternative<MoveAction>(actionTaken)) {
    MoveAction &action = std::get<MoveAction>(actionTaken);
    map.RemoveOwnership(l, action.prevCoord);
    map.AddOwnership(l);
  }
  if (std::holds_alternative<BuildAction>(actionTaken)) {
    BuildAction &action = std::get<BuildAction>(actionTaken);
    if (l->coordinate.x != action.prevCoord.x ||
      l->coordinate.y != action.prevCoord.y) {
      map.RemoveOwnership(l, action.prevCoord);
      map.AddOwnership(l);
    }
  }
}

void Manager::MainLoop() {
  while ((player.HasUnit(PEASANT) && player.HasStructure(HALL)) &&
         (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
    for (int i = 0; i < player.units.size(); i++) {
      if (player.units[0]->GetActionQueueSize() > 0) {
        actionT actionDone = player.units[0]->TakeAction();
        CheckForOwnership(player.units[0].get(), actionDone);
        std::cout<<enemy.units[0]->health << "\n";
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
