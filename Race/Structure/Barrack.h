#ifndef BARRACK_H
#define BARRACK_H

#include "Structure.h"
#include "../Unit/Unit.h"
#include "../Unit/Footman.h"
#include "../Unit/Hero/Archmage.h"
#include "../Unit/Hero/BloodMage.h"
#include "../Unit/Peasant.h"
#include <vector>
#include <memory>
#include <iostream>

using namespace Warcraft::Units;
using namespace Warcraft::Units::Heroes;

namespace Warcraft::Structures
{
    class Barrack : public Structure
    {
        public:
            Barrack()
            {
                name = "Barrack";
                description = "Can create units.";
                health = 1500;
                maxHealth = health;
                goldCost = 220;
                is = BARRACK;
                buildTime = 70;
            }
        
        public:
            void FinishBuilding() override
            {

            }
            std::string GetDescription() override
            {
                return "Recruit soldiers.";
            }
            void CreateUnit(std::vector<std::unique_ptr<Unit>>& units, int& playerGold, UnitType type);
    };
}
#endif

