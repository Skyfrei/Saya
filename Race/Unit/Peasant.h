#pragma once

#include "Unit.h"
#include "../Structure/Structure.h"
#include "../Structure/TownHall.h"
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
                maxHealth = health;

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
                if (WithinDistance(terr.coord))
                {
                    if (terr.type == GROUND)
                    {
                        terr.object = &stru;
                    }
                }
                else
                {
                    Move(terr.coord);
                }
            }
            void FarmGold(Terrain& terr)
            {
                if (WithinDistance(terr.coord))
                {
                    if (terr.type == GOLD)
                    {
                        if (goldInventory >= 20)
                            TransferGold();
                        if (GetAttackTime())
                            goldInventory++;
                    }
                }
                else
                {
                    Move(terr.coord);
                }
            }
            void TransferGold(TownHall& hall)
            {
                Move(hall.coordinate);

                if (WithinDistance(hall.coordinate))
                    goldInventory = 0;

            }
            std::string GetDescription() override{};
        private:

            bool WithinDistance(Vec2 terr)
            {
                Vec2 difference;
                difference.x = coordinate.x - terr.x;
                difference.y = coordinate.y - terr.y;

                if (std::abs(difference.x) == 1 && std::abs(difference.y) == 1)
                    return true;

                return false;
            }

        public:
            int goldInventory = 0;
    };
}
