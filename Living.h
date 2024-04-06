#ifndef LIVING_H
#define LIVING_H

#include <string>

#include "Tools/Vec2.h"

class Living {
 public:
  Living() {
    // id = objNumber;
    // objNumber++;
  }
  virtual ~Living() = default;
  // Living::Living(const Living&)
  // Living& Living::operator=(const Living&)
  // Living::Living(Living&&)
  // Living& Living::operator=(Living&&)

  virtual std::string GetDescription() = 0;

 public:
  bool HasEnoughGold(int playerGold, int cost);
  bool Die();

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
