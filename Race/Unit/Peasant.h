#pragma once

#include "Unit.h"
#include "../Spells/Spell.h"
#include <vector>

using namespace Warcraft::Spells;

namespace Warcraft::Units
{
    class Peasant : public Unit
    {
        public:
            Peasant()
            {
                name = "Peasant";
                description = "God damn farm workers.";
                health = 240;
                attack = 5.5;
                
                mana = 200;
                manaRegen = 0.67f;

                goldCold = 75;
                foodCost = 1;

                attackCooldown = 2.0f;
                buildTime = 15;

                is = PEASANT;
            }

        public:
            void Build();
        
            std::string GetDescription() override{};
    };
}
