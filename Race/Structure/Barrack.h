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
            void CreateUnit(std::vector<Unit*>& units, int playerGold, UnitType type)
            {
                switch (type)
                {
                    case FOOTMAN:
                        Footman* man;
                        if (HasEnoghGold(playerGold, man->goldCost) == true)
                        {
                            man->coordinate = coordinate;
                            units.push_back(man);
                        }
                        
                    case PEASANT:
                        Peasant* slave;
                        if (HasEnoghGold(playerGold, slave->goldCost) == true)
                        {
                            slave->coordinate = coordinate;
                            units.push_back(slave);
                        }

                    case ARCHMAGE:
                        Archmage* arch;
                        if (HasEnoghGold(playerGold, arch->goldCost) == true)
                        {
                            arch->coordinate = coordinate;
                            units.push_back(arch);
                        }
                        
                    case BLOODMAGE:
                        BloodMage* blood;
                        if (HasEnoghGold(playerGold, blood->goldCost) == true)
                        {
                            blood->coordinate = coordinate;
                            units.push_back(blood);
                        }
                }
            }
    };
}
