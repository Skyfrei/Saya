#pragma once

#include <iostream>
#include <string>

#include "Tools/Vec2.h"

enum Side { PLAYER, ENEMY };

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
  int health;
  int id;
  float maxHealth;
  int goldCost = 0;
  int foodCost = 0;
  int buildTime;
  Side belongs;
  Vec2 coordinate;
};
