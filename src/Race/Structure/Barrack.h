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

class Barrack : public Structure
{
  public:
    Barrack(Vec2 coord);
    Barrack(Vec2 coord, float hp);

  public:
    void FinishBuilding() override {
    }
    Structure *Clone() const override {
        return new Barrack(*this);
    }
    std::string GetDescription() override {
        return "Recruit soldiers.";
    }
    void CreateUnit(std::vector<std::unique_ptr<Unit>> &units, int &playerGold,
                    UnitType type);
};
#endif
