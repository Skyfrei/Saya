#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "../Living.h"
#include "../Map/Map.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Unit/Unit.h"
#include "../Tools/Vec2.h"
#include "Race/Structure/Structure.h"

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction>;

struct RecruitAction {
  Unit *un;
  bool operator==(const RecruitAction& other){
    return *un == *other.un;
  }
};

class Player {
 public:
  Player(Map &m);
  void Initialize();
  void SetInitialCoordinates(Vec2 v);
  bool HasStructure(StructureType structType);
  bool HasUnit(UnitType unitType);
  Structure &FindClosestStructure(Unit &unit, StructureType type);
  void ValidateFood();
  void UpdateGold(int g);
  std::unique_ptr<Structure> ChooseToBuild(StructureType structType);
  std::unique_ptr<Unit> ChooseToRecruit(UnitType);
  void RecruitSoldier(UnitType unitType, Structure *);
  std::vector<std::unique_ptr<Unit>> SelectUnits();
  void Move(Unit *, Vec2);
  void Attack(Unit *, Living *);
  void Build(Peasant *, StructureType, Vec2);
  void FarmGold(Peasant *, Vec2, TownHall *);

 public:
  int gold = 300;
  Vec2 food;
  std::vector<std::unique_ptr<Unit>> units;
  std::vector<std::unique_ptr<Structure>> structures;
  Map &map;
};

#endif
