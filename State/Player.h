#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "../Living.h"
#include "../State/Map.h"
#include "../Tools/Vec2.h"
#include "../Race/Unit/Unit.h"

class Peasant;
class Structure;
class Footman;
class TownHall;
class Barrack;
class Farm;

enum Side{
    PLAYER,
    ENEMY
};


class Player {
 public:
  Player(Map &m, Side en);	
	Player(const Player& other);
  void Initialize();
  void SetInitialCoordinates(Vec2 v);
  bool HasStructure(StructureType structType);
  bool HasUnit(UnitType unitType);
  Structure &FindClosestStructure(Unit &unit, StructureType type);
  void ValidateFood();
  void UpdateGold(int g);
  std::unique_ptr<Structure> ChooseToBuild(StructureType structType);
  std::unique_ptr<Unit> ChooseToRecruit(UnitType);
  std::vector<std::unique_ptr<Unit>> SelectUnits();
  void TakeAction(actionT action);
  void Move(Unit *, Vec2);
  void Attack(Unit *, Living *);
  void Build(Peasant *, StructureType, Vec2);
  void FarmGold(Peasant *, Vec2, TownHall *);
  void RecruitSoldier(UnitType unitType, Structure *);

 public:
  int gold = 300;
  Vec2 food;
  std::vector<std::unique_ptr<Unit>> units;
  std::vector<std::unique_ptr<Structure>> structures;
  Map &map;
  Side side;
};

#endif
