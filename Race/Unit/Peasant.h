#pragma once

#include "Unit.h"
#include "../Structure/Structure.h"
#include <vector>

using namespace Warcraft::Structures;


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
            void Build(Structure& stru);
            void FarmGold();
            void TransferGold();
        
            std::string GetDescription() override{};

        public:
            int goldInventory = 0;
    };
}
