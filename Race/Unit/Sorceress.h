#pragma once

#include "Unit.h"

#include <vector>

namespace Warcraft::Units
{
    class Sorceress : public Unit
    {
        public:
            Sorceress()
            {
                name = "Sorceress";
                description = "Queen of magic.";
                health = 325;
                attack = 11;
                mana = 200;
                manaRegen = 0.67f;

                goldCold = 175;
                foodCost = 2;

                attackCooldown = 1.7;
                buildTime = 30;

                is = OTHER;
            }
        
            std::string GetDescription() override{};
    };
}
