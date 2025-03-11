#ifndef BARRACK_H
#define BARRACK_H

#include <iostream>
#include <memory>
#include <vector>

#include "../Unit/Footman.h"
#include "../Unit/Hero/Archmage.h"
#include "../Unit/Hero/BloodMage.h"
#include "../Unit/Peasant.h"
#include "../Unit/Unit.h"
#include "Structure.h"

class Barrack : public Structure {
 public:
  Barrack() {
    name = "Barrack";
    description = "Can create units.";
    health = 1500;
    maxHealth = health;
    goldCost = 220;
    is = BARRACK;
    buildTime = 70;
  }

 public:
  void FinishBuilding() override {}
  std::unique_ptr<Structure> Clone() const override{
    return std::make_unique<Barrack>(*this);
  }
  std::string GetDescription() override { return "Recruit soldiers."; }
  void CreateUnit(std::vector<std::unique_ptr<Unit>> &units, int &playerGold,
                  UnitType type);
};
#endif
