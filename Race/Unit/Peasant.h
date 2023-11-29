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

                goldCost = 75;
                foodCost = 1;

                attackCooldown = 2.0f;
                buildTime = 15;

                is = PEASANT;
            }

        public:
           void Build(Structure& stru)
            {
                
            }
            void FarmGold(int upkeep)
            {
                // based on upkeep of player
                if (goldInventory >= 100)
                    // cant farm gold anymore
                    TransferGold();

                goldInventory++;

            }
            void TransferGold()
            {
                // start walking back
                // once it reaches town hall
                goldInventory = 0;
            }
            std::string GetDescription() override{};

        public:
            int goldInventory = 0;
    };
}
