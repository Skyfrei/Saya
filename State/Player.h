#include "../Tools/Vec2.h"
#include <vector>
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/TownHall.h"
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

        public:
            int gold;
            Vec2 food;

            std::vector<Unit*> units;
            std::vector<Structure*> structures;
            std::vector<Living*> all;
    };
}

