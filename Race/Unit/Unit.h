#pragma once
#include <chrono>
#include <cstddef>
#include <variant>
#include <vector>

#include "../../Living.h"
#include "../../Map/Map.h"
#include "../../Tools/Vec2.h"
#include "../Spells/Spell.h"
#include "Map/Terrain.h"
#include "Race/Structure/Structure.h"
#include "Race/Structure/TownHall.h"
using namespace std::chrono;

enum UnitType { FOOTMAN, PEASANT, ARCHMAGE, BLOODMAGE };

enum MoveType { W, NW, N, NE, E, SE, S, SW, STAY };

enum ActionType{MOVE, ATTACK, BUILD, FARMGOLD, RECRUIT, NR_OF_ACTIONS};

struct Path {
  Path(int d, Vec2 l) : distance(d), comesFrom(l) {}
  Path() {}
  int distance;
  Vec2 comesFrom;
};

class Action{
  public:
    ActionType type;
    virtual ActionType GetType() = 0;
};

struct MoveAction : public Action {
  Vec2 prevCoord;
  Vec2 destCoord;
  ActionType type = MOVE;
  MoveAction(Vec2 c) : destCoord(c) {}

  ActionType GetType() override {return type;}

  bool operator==(const MoveAction &b) const {
    if (destCoord.x == b.destCoord.x && destCoord.y == b.destCoord.y)
      return true;
    return false;
  }
};
struct AttackAction : public Action{
  AttackAction() {}
  AttackAction(Living *o) : object(o) {}
  Vec2 prevCoord;
  ActionType type = ATTACK;
  ActionType GetType() override {return type;}

  Living *object;
  constexpr bool operator==(const AttackAction &b) const {
    return object == b.object;
  }
};
struct BuildAction : public Action{
  Structure *stru;
  Vec2 prevCoord;
  BuildAction(Structure *s) : stru(s) {}
  ActionType type = BUILD;
  ActionType GetType() override {return type;}


  bool operator==(const BuildAction &b) const {
    if (stru->coordinate.x == b.stru->coordinate.x &&
        stru->coordinate.y == b.stru->coordinate.y && stru == b.stru)
      return true;
    return false;
  }
};
struct FarmGoldAction : public Action {
  Vec2 dest;
  Vec2 prev;
  Terrain *terr;
  TownHall *hall;
  int gold = 0;
  FarmGoldAction(Vec2 v, Terrain *te, TownHall *t)
      : dest(v), terr(te), hall(t) {}

  ActionType type = FARMGOLD;
  ActionType GetType() override {return type;}
  bool operator==(const FarmGoldAction &a) const {
    if (a.dest.x == dest.x && a.dest.y == dest.y) return true;
    return false;
  }
};

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction>;

class Unit : public Living {
 public:
  Unit();
  virtual ~Unit() = default;
  virtual std::unique_ptr<Unit> Clone() const = 0;

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
  bool operator==(const Unit &other) const;

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
