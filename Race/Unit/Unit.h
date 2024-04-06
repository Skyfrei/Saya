#pragma once
#include <chrono>
#include <variant>
#include <vector>

#include "../../Living.h"
#include "../../Map/Map.h"
#include "../../Tools/Vec2.h"
#include "../Spells/Spell.h"
#include "Race/Structure/Structure.h"

using namespace std::chrono;

enum UnitType { FOOTMAN, PEASANT, ARCHMAGE, BLOODMAGE };

enum MoveType { W, NW, N, NE, E, SE, S, SW, STAY };

using actionT =
    std::variant<std::shared_ptr<Living>, Vec2, std::shared_ptr<Structure>>;
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
  void Attack(Living &un);
  void RegenHealth();
  void MoveDij(Vec2 dest, Map &map);
  bool WithinDistance(Vec2 terr);
  bool CanAttack();
  Vec2 FindDifference(Vec2 terr);
  void TakeAction();
  void InsertAction(actionT);
  void ResetActions();
  bool HasCommand();
  int GetActionQueueSize();

 private:
  bool IsMovable();
  void MoveCoordinates(Vec2 terr);
  void ChangeCoordinate(MoveType dir);
  void InsertAction(std::variant<Living *, Vec2>);

 public:
  bool isInvisible = false;
  float attack{};
  float mana = 100;
  int maxMana = 100;
  float attackRange{};
  int movementSpeed = 1;
  float attackCooldown = 0.35f;
  float hpCooldown = 0.35f;
  float moveCooldown = 0.25f;
  float manaRegen = 0.25;
  float hpRegen = 0.25f;

  UnitType is;

 private:
  high_resolution_clock::time_point hpTime = high_resolution_clock::now();
  high_resolution_clock::time_point attackTime = high_resolution_clock::now();
  high_resolution_clock::time_point moveTime = high_resolution_clock::now();
  std::vector<actionT> actionQueue;
};
