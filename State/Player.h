#pragma once

#include "../Tools/Vec2.h"
#include <memory>
#include <vector>
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Unit/Unit.h"
#include <memory>
#include "../Living.h"
#include "../Map/Map.h"

using namespace Warcraft::Units;
using namespace Warcraft::Structures;
using namespace Warcraft;
using namespace Warcraft::Environment;

namespace Warcraft::State
{
    class Player
    {
        public: 
            Player()
            {
                food.y = 10;
                gold = 300;
            }
            void Initialize(Map& m)
            {
                structures.push_back(std::make_unique<TownHall> ());
                for (int i = 0; i < 5; i++)
                {
                    units.push_back(std::make_unique<Peasant>());
                    units[0]->coordinate = structures[0]->coordinate;
                }
                structures.push_back(std::make_unique<Barrack> ());
                map = m;
                ValidateFood();
            }
            void SetInitialCoordinates(Vec2 v)
            {
                for (const auto & structure : structures)
                {
                    structure->coordinate = v;
                }
                for(const auto & unit : units)
                {
                    unit->coordinate = structures[0]->coordinate;
                }
            }

            bool HasStructure(StructureType structType)
            {
                for (int i = 0; i < structures.size(); i++)
                {
                    if (structures[i]->is == structType)
                    {
                        return true;
                    }
                }
                return false;
            }

            bool HasUnit(UnitType unitType)
            {
                for (const auto & unit : units)
                {
                    if (unit->is == unitType)
                    {
                        return true;
                    }
                }
                return false;
            }

            Living& FindClosestLiving(Unit& unit, StructureType type)
            {
                double min = 100;
                int8_t index = 0;

                for(int i  = 0; i < structures.size(); i++)
                {
                    if (structures[i]->is == type) {
                        Vec2 difference = unit.FindDifference(structures[i]->coordinate);
                        double temp = std::sqrt(std::pow(difference.x, 2) + std::pow(difference.y, 2));
                        if (temp <= min)
                        {
                            min = temp;
                            index = i;
                        }
                    }
                }

                return *structures[index];
            }

            void ValidateFood()
            {
                food.x = 0;
                food.y = 0;
                
                for(const auto & unit : units)
                {
                    food.x += unit->foodCost;
                }
                for (const auto & structure : structures)
                {
                    if (structure->is == FARM)
                        food.y += 5;
                }
            }

            void UpdateGold(int g)
            {
                gold += g;
            }

            void ChooseToBuild(StructureType structType, Vec2 terrCoord)
            {
                for (const auto & unit : units)
                {
                    if (unit->is == PEASANT)
                    {
                        dynamic_cast<Peasant&>(*unit).Build(structures, gold, structType, map.objects[terrCoord.x][terrCoord.y]);
                    }
                }
            }

            void RecruitSoldier(UnitType unitType)
            {
                if (HasStructure(BARRACK))
                {
                    if (unitType == ARCHMAGE)
                    {
                        bool temp = HasUnit(BLOODMAGE);
                        if (temp)
                        {
                            return;
                        }
                    }
                    else if (unitType == BLOODMAGE)
                    {
                        bool temp = HasUnit(ARCHMAGE);
                        if (temp)
                        {
                            return;
                        }
                    }
                }

                for (const auto & structure : structures)
                {
                    if (structure->is == BARRACK)
                    {
                        dynamic_cast<Barrack&>(*structure).CreateUnit(units, gold, unitType);

                    }
                }
            }


        public:
            int gold;
            Vec2 food;

            std::vector<std::unique_ptr<Unit>> units;
            std::vector<std::unique_ptr<Structure>> structures;
        private:
            Map map;
    };
}

