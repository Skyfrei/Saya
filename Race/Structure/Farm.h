#pragma once
#include <string>

#include "Structure.h"

class Farm : public Structure {
 public:
  Farm() {
    name = "Farm";
    description = "Gives 6 food.";
    health = 500;
    maxHealth = health;

    goldCost = 100;

    buildTime = 10;
    is = FARM;
  }

 public:
  std::string GetDescription() override { return description; }
  void FinishBuilding() override {}
  std::unique_ptr<Structure> Clone() const override{
    return std::make_unique<Farm>(*this);
  }

 public:
  int GetFood() { return 5; }
};
