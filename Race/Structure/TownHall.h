#pragma once
#include "Structure.h"

enum Upgrade { WOOD, BRONZE, IRON, STEEL, MITHRIL };

class TownHall : public Structure {
 public:
  TownHall() {
    name = "Town Hall";
    description = "Can recruit peasants.";
    health = 1500;
    maxHealth = health;

    goldCost = 590;
    is = HALL;
    buildTime = 180;

    description = "Can build unit upgrades.";
  }

 public:
  Upgrade currentUpgrade = WOOD;
  void UpgradeEquipment(Upgrade &curr) {

  };
  void FinishBuilding() override {}
  std::unique_ptr<Structure> Clone() const override{
    return std::make_unique<TownHall>(*this);
  }
  std::string GetDescription() override { return "Town hall."; }
};
