#pragma once

#include "Unit.h"
#include "../Structure/Structure.h"
#include "../../Map/Map.h"
#include "../../Map/Terrain.h"
#include <vector>

using namespace Warcraft::Structures;
using namespace Warcraft::Environment;

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
           void Build(Structure& stru, Terrain& terr)
            {
                if (coordinate.x + 1 <= terr.coord.x && coordinate.y + 1 <= terr.coord.y)
                {
                    if (terr.type == GROUND)
                    {

                    }
                }
                else
                {
                    FindShortestPath(terr.coord);
                }
            }
            void FarmGold(Terrain& terr)
            {
                if (coordinate.x == terr.coord.x && coordinate.y == terr.coord.y)
                {
                    if (terr.type == GOLD)
                    {
                        if (goldInventory >= 20)
                        // cant farm gold anymore
                        TransferGold();

                        goldInventory++;
                    }
                }
                else
                {
                    FindShortestPath(terr.coord);
                }
            }
            void TransferGold()
            {
                // Find closest town hall
                // start walking back
                goldInventory = 0;
            }
            std::string GetDescription() override{};

        public:
            int goldInventory = 0;
    };
}
