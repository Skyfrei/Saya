#pragma once
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../../Living.h"
#include "../../Map/Graph.h"
#include "../../Map/Map.h"
#include "../../Map/Terrain.h"
#include "../../Tools/Vec2.h"
#include "../Spells/Spell.h"
#include "../Structure/Structure.h"

enum UnitType { FOOTMAN, PEASANT, ARCHMAGE, BLOODMAGE };

enum MoveType { W, NW, N, NE, E, SE, S, SW, STAY };

struct Path {
  Path(int d, Vec2 l) : distance(d), comesFrom(l) {}
  Path() {}
  int distance;
  Vec2 comesFrom;
};

class Unit : public Living {
 public:
  Unit();
  virtual ~Unit() = default;

 public:
  void Move(Vec2 terr);
  bool WithinDistance(Vec2 terr);
  Vec2 FindDifference(Vec2 terr);
  void Attack(Living &un);
  bool GetAttackTime();
  void RegenHealth();
  void MoveDij(Vec2 dest, Map &map);
  // void CheckTask(Task<Unit>& t);
  void ChangeCoordinate(MoveType dir);

 public:
  bool isInvisible = false;
  float attack{};
  float attackCooldown;
  float mana = 100;
  float manaRegen = 0.25;
  int maxMana = 100;
  float attackRange{};
  int movementSpeed = 1;
  float hpRegen = 0.25f;
  UnitType is;
  std::chrono::high_resolution_clock::time_point time1 =
      std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point hpTime =
      std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::time_point time =
      std::chrono::high_resolution_clock::now();
};
