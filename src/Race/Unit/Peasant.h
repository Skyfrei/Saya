#ifndef PEASANT_H
#define PEASANT_H
#include <memory>

#include "../../Tools/Enums.h"
#include "../../Tools/Vec2.h"
#include "Unit.h"

class Peasant : public Unit {
 public:
    Peasant();
    Peasant(Vec2 coord, float hp, float man);

 public:
  void Build(Structure *);
  void FarmGold(Terrain &terr, TownHall &hall, int &g);
  std::string GetDescription() override { return "Slave."; }
  std::unique_ptr<Unit> Clone() const override{
    return std::make_unique<Peasant>(*this);
  }

 public:
  int goldInventory = 0;
  int maxGoldInventory = 10;

 private:
};

#endif
