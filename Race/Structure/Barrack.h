#pragma once
#include "Structure.h"
#include "../Unit/Unit.h"
#include "../Unit/Peasant.h"
#include "../Unit/Footman.h"
#include "../Unit/Hero/Archmage.h"
#include "../Unit/Hero/BloodMage.h"
#include <vector>

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
            bool HasEnoghGold(int playerGold, int cost)
            {
                if (playerGold >= cost)
                    return true;

                return false;
            }
            void CreateUnit(std::vector<std::unique_ptr<Unit>>& units, int playerGold, UnitType type)
            {
                switch (type)
                {
                    case FOOTMAN:
                        std::unique_ptr<Footman> man(new Footman());
                        if (HasEnoghGold(playerGold, man->goldCost) == true)
                        {
                            man->coordinate = coordinate;
                            units.emplace_back(std::move(man));
                        }
                        break;
                        
                    case PEASANT:
                        std::unique_ptr<Peasant> slave(new Peasant());
                        if (HasEnoghGold(playerGold, slave->goldCost) == true)
                        {
                            slave->coordinate = coordinate;
                            units.push_back(std::move(slave));
                        }
                        break;

                    case ARCHMAGE:
                        std::unique_ptr<Archmage> arch(new Archmage());
                        if (HasEnoghGold(playerGold, arch->goldCost) == true)
                        {
                            arch->coordinate = coordinate;
                            units.push_back(std::move(arch));
                        }
                        break;
                        
                    case BLOODMAGE:
                        std::unique_ptr<BloodMage> blood(new BloodMage());
                        if (HasEnoghGold(playerGold, blood->goldCost) == true)
                        {
                            blood->coordinate = coordinate;
                            units.push_back(std::move(blood));
                        }
                        break;
                }
            }
    };
}
