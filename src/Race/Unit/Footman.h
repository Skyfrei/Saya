#ifndef FOOTMAN_H
#define FOOTMAN_H
#include <string>

#include "Unit.h"

class Footman : public Unit {
 public:
  Footman() {
    name = "Footman";
    description = "A normal foot soldier.";
    health = 420;
    maxHealth = health;
    attack = 12.5;

    goldCost = 135;
    foodCost = 2;

    buildTime = 20;

    is = FOOTMAN;
  }
  std::string GetDescription() override { return "Footman."; };
  std::unique_ptr<Unit> Clone() const override{
    return std::make_unique<Footman>(*this);
  }

};
#endif
