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

class Player {
 public:
  Player(Map &m);
  Player();
  void Initialize(Map &m);
  void SetInitialCoordinates(Vec2 v);
  bool HasStructure(StructureType structType);
  bool HasUnit(UnitType unitType);
  Structure &FindClosestStructure(Unit &unit, StructureType type);
  void ValidateFood();
  void UpdateGold(int g);
  void ChooseToBuild(StructureType structType, Vec2 terrCoord);
  void RecruitSoldier(UnitType unitType);
  void Move(Unit &un, Vec2 terr);
  std::vector<std::shared_ptr<Unit>> SelectUnits();

 public:
  int gold;
  Vec2 food;
  std::vector<std::shared_ptr<Unit>> units;
  std::vector<std::shared_ptr<Structure>> structures;
  Map map;
};

#endif
