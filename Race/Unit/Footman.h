#pragma once
#include "Unit.h"
#include <string>

namespace Warcraft::Units
{
    class Footman : public Unit
    {
        public:
            Footman()
            {
                name = "Footman";
                description = "A normal foot soldier.";
                health = 420;
                maxHealth = health;
                attack = 12.5;

                goldCost = 135;
                foodCost = 2;

                attackCooldown = 1.35;
                buildTime = 20;

                is = FOOTMAN;
            }
            std::string GetDescription() override{ return "Footman.";};
    };
}
