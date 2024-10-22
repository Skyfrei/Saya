#ifndef UNIT_H
#define UNIT_H

#include <chrono>
#include <cstddef>
#include <variant>
#include <vector>

#include "../../Living.h"
#include "../Structure/Structure.h"
#include "../Spells/Spell.h"
#include "../../State/Terrain.h"

using namespace std::chrono;

class TownHall;
class Farm;
class Barrack;
class Unit;

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
  Unit* unit;
  ActionType type = MOVE;

  MoveAction(Vec2 c) : destCoord(c) {}
  MoveAction(Unit* un, Vec2 c) : destCoord(c), unit(un){}

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
  AttackAction(Unit* un, Living* o) : object(o), unit(un){}
  Vec2 prevCoord;
  Unit* unit;
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
  StructureType struType;
  Vec2 coordinate;
  Unit* peasant;
  BuildAction(Structure *s) : stru(s) {}
  BuildAction(Unit* p, StructureType s, Vec2 c) : peasant(p), struType(s), coordinate(c){} 
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
  Unit* peasant;
  int gold = 0;
  FarmGoldAction(Vec2 v, Terrain *te, TownHall *t)
      : dest(v), terr(te), hall(t) {}
  FarmGoldAction(Unit *p, Vec2 v, TownHall *h) : peasant(p), dest(v), hall(h){}

  ActionType type = FARMGOLD;
  ActionType GetType() override {return type;}
  bool operator==(const FarmGoldAction &a) const {
    if (a.dest.x == dest.x && a.dest.y == dest.y) return true;
    return false;
  }
};

struct RecruitSoldierAction : public Action{
  UnitType unitType;
  Structure *stru;
  ActionType type = RECRUIT;
  ActionType GetType() override {return type;}
  bool operator==(const RecruitSoldierAction &a) const {
    if (stru == a.stru && unitType == a.unitType) return true;
    return false;
    
  }
  RecruitSoldierAction(UnitType type, Structure* s) : unitType(type), stru(s){}
};
struct RecruitAction {
  Unit *un;
  bool operator==(const RecruitAction& other){
    return un == other.un;
  }
};

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction, RecruitSoldierAction>;

class Unit : public Living {
 public:
  Unit();
  virtual ~Unit() = default;
  virtual std::unique_ptr<Unit> Clone() const = 0;

 public:
  void Move(Vec2 terr);
  void Attack(Living &un);
  void RegenHealth();
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
  duration<float, std::milli> attackCooldown = std::chrono::milliseconds(200);
  duration<float, std::milli> hpCooldown = std::chrono::milliseconds(1000);
  duration<float, std::milli> moveCooldown = std::chrono::milliseconds(1500);
  float manaRegen = 0.25f;
  float hpRegen = 0.25f;

  UnitType is;

 private:
  high_resolution_clock::time_point hpTime = high_resolution_clock::now();
  high_resolution_clock::time_point attackTime = high_resolution_clock::now();
  high_resolution_clock::time_point moveTime = high_resolution_clock::now();
  std::vector<actionT> actionQueue;
};

#endif
