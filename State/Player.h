#include "../Tools/Vec2.h"
#include <vector>
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Unit/Unit.h"
#include <memory>
#include "../Living.h"

using namespace Warcraft::Units;
using namespace Warcraft::Structures;
using namespace Warcraft;

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
            void Initialize()
            {
                structures.push_back(std::unique_ptr<TownHall> (new TownHall()));
                for (int i = 0; i < 5; i++)
                {
                    units.push_back(std::unique_ptr<Peasant>(new Peasant()));
                    units[0]->coordinate = structures[0]->coordinate;
                }


                structures.push_back(std::unique_ptr<Barrack> (new Barrack()));
                ValidateFood();
                
            }
            void SetInitialCoordinates(Vec2 v)
            {
                for (int i = 0; i < structures.size(); i++)
                {
                    structures[i]->coordinate = v;
                }
                for(int i = 0; i < units.size(); i++)
                {
                    units[i]->coordinate = structures[0]->coordinate;
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
                for (int i = 0; i < units.size(); i++)
                {
                    if (units[i]->is == unitType)
                    {
                        return true;
                    }
                }
                return false;
            }

            void ValidateFood()
            {
                food.x = 0;
                food.y = 0;
                
                for(int i = 0; i < units.size(); i++)
                {
                    food.x += units[i]->foodCost;
                }
                for (int i = 0; i < structures.size(); i++)
                {
                    if (structures[i]->is == FARM)
                        food.y += 5;
                }
            }

            void UpdateGold(int g)
            {
                gold += g;
            }

            // void IncreaseFoodCapac()
            // {
            //     if (food.y + f.GetFood() >= 60 )
            //         return;
                    
            //     food.y += f.GetFood();
            // }

            void ChooseToBuild()
            {

            }

            void RecruitSoldier(UnitType unitType)
            {
                if (HasStructure(BARRACK) == true)
                {
                    if (unitType == ARCHMAGE)
                    {
                        bool temp = HasUnit(BLOODMAGE);
                        if (temp == true)
                        {
                            return;
                        }
                    }
                    else if (unitType == BLOODMAGE)
                    {
                        bool temp = HasUnit(ARCHMAGE);
                        if (temp == true)
                        {
                            return;
                        }
                    }
                }

                for (int i = 0; i < structures.size(); i++)
                {
                    if (structures[i]->is == BARRACK)
                    {
                        static_cast<Barrack&>(*structures[i]).CreateUnit(units, gold, unitType);
                        
                    }
                }
            }


        public:
            int gold;
            Vec2 food;

            std::vector<std::unique_ptr<Unit>> units;
            std::vector<std::unique_ptr<Structure>> structures;
            std::vector<std::unique_ptr<Living>> all;
    };
}

