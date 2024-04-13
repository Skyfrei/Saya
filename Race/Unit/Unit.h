#pragma once
#include <chrono>
#include <cstddef>
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

struct Path {
  Path(int d, Vec2 l) : distance(d), comesFrom(l) {}
  Path() {}
  int distance;
  Vec2 comesFrom;
};
struct MoveAction {
  Vec2 prevCoord;
  Vec2 destCoord;
  MoveAction(Vec2 c) : destCoord(c) {}
  bool operator==(const MoveAction &b) const {
    if (destCoord.x == b.destCoord.x && destCoord.y == b.destCoord.y)
      return true;
    return false;
  }
};
struct AttackAction {
  AttackAction() {}
  AttackAction(Living *o) : object(o) {}
  Vec2 prevCoord;

  Living *object;
  constexpr bool operator==(const AttackAction &b) const {
    if (object == b.object) return true;
    return false;
  }
};
struct BuildAction {
  Vec2 coord;
  Structure *stru;
  Vec2 prevCoord;
  BuildAction(Structure *s, Vec2 c) : stru(s), coord(c) {}
  bool operator==(const BuildAction &b) const {
    if (coord.x == b.coord.x && coord.y == b.coord.y && stru == b.stru)
      return true;
    return false;
  }
};

using actionT = std::variant<AttackAction, MoveAction, BuildAction>;

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
  actionT TakeAction();
  void InsertAction(actionT);
  void ResetActions();
  bool HasCommand();
  int GetActionQueueSize();

 private:
  bool IsMovable();
  void MoveCoordinates(Vec2 terr);
  void ChangeCoordinate(MoveType dir);

 public:
  bool isInvisible = false;
  float attack{};
  float mana = 100;
  int maxMana = 100;
  float attackRange{};
  int movementSpeed = 1;
  duration<float> attackCooldown = duration<float>(0.40f);
  duration<float> hpCooldown = duration<float>(0.35f);
  duration<float> moveCooldown = duration<float>(0.35f);
  float manaRegen = 0.25f;
  float hpRegen = 0.25f;

  UnitType is;

 private:
  high_resolution_clock::time_point hpTime = high_resolution_clock::now();
  high_resolution_clock::time_point attackTime = high_resolution_clock::now();
  high_resolution_clock::time_point moveTime = high_resolution_clock::now();
  std::vector<actionT> actionQueue;
};
