#pragma once

#include "Unit.h"
#include "../Structure/Structure.h"
#include "../Structure/TownHall.h"
#include "../Structure/Barrack.h"
#include "../Structure/Farm.h"
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
           void Build(std::vector<std::unique_ptr<Structure>>& structures, int& playerGold, StructureType type, Terrain& terr)
            {
                std::unique_ptr<Structure> struc;
                if (WithinDistance(terr.coord))
                {
                    if (terr.type == GROUND)
                    {
                        switch (type)
                        {
                            case BARRACK:
                                struc = std::make_unique<Barrack>();
                                break;

                            case FARM:
                                struc =  std::make_unique<Farm>();
                                break;

                            case HALL:
                                struc = std::make_unique<TownHall>();
                                break;

                            case OTHER:
                                break;
                        }
                        if (HasEnoughGold(playerGold, struc->goldCost))
                        {
                            playerGold -= struc->goldCost;
                            structures.emplace_back(std::move(struc));
                            //terr.object = stru;
                        }
                    }
                }
                else
                {
                    Move(terr.coord);
                }
            }
            void FarmGold(Terrain& terr, TownHall& hall)
            {
                if (WithinDistance(terr.coord))
                {
                    if (terr.type == GOLD)
                    {
                        if (goldInventory >= 20)
                            TransferGold(hall);
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

        public:
            int goldInventory = 0;
    };
}
