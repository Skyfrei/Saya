#ifndef PEASANT_H
#define PEASANT_H
#include <deque>
#include <memory>
#include <vector>

#include "../../Map/Map.h"
#include "../../Map/Terrain.h"
#include "../Structure/Barrack.h"
#include "../Structure/Farm.h"
#include "../Structure/Structure.h"
#include "../Structure/TownHall.h"
#include "Unit.h"

class Peasant : public Unit {
 public:
  Peasant() {
    name = "Peasant";
    description = "God damn farm workers.";
    health = 240;
    maxHealth = health;

    attack = 5.5;

    mana = 200;
    manaRegen = 0.67f;

    goldCost = 75;
    foodCost = 1;

    buildTime = 15;

    is = PEASANT;
  }

 public:
  void Build(std::vector<std::unique_ptr<Structure>> &structures,
             int &playerGold, StructureType type, Terrain &terr);
  void FarmGold(Terrain &terr, TownHall &hall);
  void TransferGold(TownHall &hall);
  std::string GetDescription() override { return "Slave."; }

 public:
  int goldInventory = 0;

 private:
};

#endif
