#include "../Tools/Vec2.h"
#include <vector>
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Unit/Unit.h"
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
                units.push_back(new Peasant());
                units.push_back(new Peasant());
                units.push_back(new Peasant());
                units.push_back(new Peasant());
                units.push_back(new Peasant());

                structures.push_back(new TownHall());
                
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
                for(int i = 0; i < units.size(); i++)
                {
                    food.x += units[i]->foodCost;
                }
            }

            void UpdateGold(int g)
            {
                gold += g;
            }

            void Move(std::vector<Unit*> uns)
            {
                
            }

            void IncreaseFoodCapac(Farm& f)
            {
                if (food.y + f.GetFood() >= 60 )
                    return;
                    
                food.y += f.GetFood();
            }
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
            }

        public:
            int gold;
            Vec2 food;

            std::vector<Unit*> units;
            std::vector<Structure*> structures;
            std::vector<Living*> all;
    };
}

