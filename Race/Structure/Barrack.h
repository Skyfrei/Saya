#pragma once
#include "Structure.h"
#include "../Unit/Unit.h"
#include "../Unit/Peasant.h"
#include "../Unit/Footman.h"
#include "../Unit/Hero/Archmage.h"
#include "../Unit/Hero/BloodMage.h"
#include <vector>
#include <iostream>

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
            bool HasEnoughGold(int playerGold, int cost)
            {
                if (playerGold >= cost)
                    return true;
                return false;
            }
            void FinishBuilding() override
            {

            }
            std::string GetDescription() override
            {
                return "Recruit soldiers.";
            }
            void CreateUnit(std::vector<std::unique_ptr<Unit>>& units, int& playerGold, UnitType type)
            {
                std::unique_ptr<Unit> unit;

                switch (type)
                {
                    case FOOTMAN:
                        unit = std::make_unique<Footman>();
                        break;
                        
                    case PEASANT:
                        unit =  std::make_unique<Peasant>();
                        break;

                    case ARCHMAGE:
                        unit = std::make_unique<Archmage>();
                        break;
                        
                    case BLOODMAGE:
                        unit = std::make_unique<BloodMage>();
                        break;
                }   
                if (HasEnoughGold(playerGold, unit->goldCost) == true)
                {
                    //std::cout<<playerGold - unit->goldCost;
                    playerGold -= unit->goldCost;
                    units.emplace_back(std::move(unit));                    
                }
            }
    };
}
