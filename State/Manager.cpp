//
// Created by Klavio Tarka on 13.12.23.
//
#include "Manager.h"

#include <chrono>
#include <memory>
#include <variant>

#include "Race/Structure/Structure.h"
#include "Race/Structure/TownHall.h"
#include "Race/Unit/Peasant.h"
#include "Race/Unit/Unit.h"

Manager::Manager() : player(map), enemy(map) {
  // game start

  player.SetInitialCoordinates(Vec2(8, 2));
  enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));

  Peasant &p = static_cast<Peasant &>(*player.units[0].get());
  TownHall &h = static_cast<TownHall &>(*player.structures[0].get());
  player.FarmGold(&p, Vec2(10, 10), &h);
  MainLoop();
}

void Manager::CheckForOwnership(Player &p, Living *l, actionT actionTaken) {
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
  } else if (std::holds_alternative<MoveAction>(actionTaken)) {
    MoveAction &action = std::get<MoveAction>(actionTaken);
    map.RemoveOwnership(l, action.prevCoord);
    map.AddOwnership(l);
  } else if (std::holds_alternative<BuildAction>(actionTaken)) {
    BuildAction &action = std::get<BuildAction>(actionTaken);
    if (l->coordinate.x != action.prevCoord.x ||
        l->coordinate.y != action.prevCoord.y) {
      map.RemoveOwnership(l, action.prevCoord);
      map.AddOwnership(l);
    }
  } else if (std::holds_alternative<FarmGoldAction>(actionTaken)) {
    FarmGoldAction &action = std::get<FarmGoldAction>(actionTaken);
    auto s = static_cast<Peasant *>(l);
    if (l->coordinate.x != action.prev.x || l->coordinate.y != action.prev.y) {
      map.RemoveOwnership(l, action.prev);
      map.AddOwnership(l);
    }
    if (s->WithinDistance(action.hall->coordinate)) {
      p.gold += action.gold;
      action.gold = 0;
    }
  }
}

void Manager::MainLoop() {
  while ((player.HasUnit(PEASANT) && player.HasStructure(HALL)) &&
         (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
    for (int i = 0; i < player.units.size(); i++) {
      if (player.units[i]->GetActionQueueSize() > 0) {
        actionT actionDone = player.units[i]->TakeAction();
        CheckForOwnership(player, player.units[i].get(), actionDone);
      }
      for (int i = 0; i < enemy.units.size(); i++) {
        if (enemy.units[i]->GetActionQueueSize() > 0) {
          actionT actionDone = enemy.units[i]->TakeAction();
          CheckForOwnership(enemy, enemy.units[i].get(), actionDone);
        }
      }
      if (Is10thSecond()) {
        trainerManager.StartPolicy(map, player, enemy);
      }
    }
  }
}

void Manager::CheckForMovement() {}

bool Manager::Is10thSecond() {
  std::chrono::high_resolution_clock::time_point frameTimer1 =
  std::chrono::high_resolution_clock::now();
      

  std::chrono::duration<float> diff = frameTimer1 - frameTimer;
  if (diff.count() >= 0.1f) {
    frameTimer = std::chrono::high_resolution_clock::now();
    return true;
  }
  return false;
}

float Manager::GetTime() {
  time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> diff = time - time1;
  return diff.count();
}

void Manager::ManageLiving(Player &pl) { std::cout << "dead"; }
