#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "../Living.h"
#include "../Race/Unit/Unit.h"
#include "../State/Map.h"
#include "../Tools/Vec2.h"
#include "../Tools/Enums.h"
#include "../Race/Unit/Action.h"

class Peasant;
class Structure;
class Footman;
class TownHall;
class Barrack;
class Farm;

class Player
{
  public:
    Player(Map &m, Side en);
    Player(const Player &other);
    ~Player();
    void Initialize();
    void SetInitialCoordinates(Vec2 v);
    bool HasStructure(StructureType structType);
    bool HasUnit(UnitType unitType);
    Structure &FindClosestStructure(Unit &unit, StructureType type);
    void ValidateFood();
    void UpdateGold(int g);
    std::unique_ptr<Structure> ChooseToBuild(StructureType structType, Vec2& coord);
    std::unique_ptr<Unit> ChooseToRecruit(UnitType);
    std::vector<std::unique_ptr<Unit>> SelectUnits();
    float TakeAction(actionT& action);
    void Move(MoveAction& action);
    void Attack(AttackAction& action);
    void Build(BuildAction& action);
    void FarmGold(FarmGoldAction& action);
    void Recruit(RecruitAction& action);

  public:
    int gold = 300;
    Vec2 food;
    std::vector<std::unique_ptr<Unit>> units;
    std::vector<std::unique_ptr<Structure>> structures;
    Map &map;
    Side side;
};

#endif
