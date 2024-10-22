#ifndef LIVING_H
#define LIVING_H

#include <string>
#include "Tools/Vec2.h"

enum StructureType { HALL, BARRACK, FARM, NR_OF_STRUCTS, OTHER };
enum UnitType { FOOTMAN, PEASANT, NR_OF_UNITS, ARCHMAGE, BLOODMAGE };
enum MoveType { W, NW, N, NE, E, SE, S, SW, STAY };
enum ActionType{MOVE, ATTACK, BUILD, FARMGOLD, RECRUIT, NR_OF_ACTIONS};

class Living {
 public:
  Living(){}
  virtual ~Living() = default;
  virtual std::string GetDescription() = 0;

 public:
  bool HasEnoughGold(int playerGold, int cost);
  bool IsDead();

 public:
  std::string name;
  std::string description;
  float health;
  int id;
  float maxHealth;
  int goldCost = 0;
  int foodCost = 0;
  int buildTime;
  Vec2 coordinate;
};

#endif
