#ifndef LIVING_H
#define LIVING_H

#include <string>
#include "Tools/Vec2.h"
#include "Tools/Enums.h"

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
