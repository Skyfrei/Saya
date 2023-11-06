#include "../Tools/Vec2.h"
#include <vector>
#include "../Unit/Unit.h"
#include "../Unit/Footman.h"
#include "../Unit/Sorceress.h"
#include "../Structure/Farm.h"
#include "../Structure/TownHall.h"
#include "../Unit/Unit.h"
#include "../Living.h"

using Warcraft::Units;

namespace Warcraft::State
{
    enum Upkeep
    {
        NIE,
        LOW,
        HIGH
    };

    class Player
    {
        public: 
            Player()
            {
                food.y = 10;
                gold = 300;
            }

            void Initialize();
            void ValidateFood();
            void ValidateUpkeep();
            void UpdateGold(int g);
            void Move();
            void IncreaseFoodCapac();

        public:
            int gold;
            Vec2 food;
            Upkeep upkeep = NIE;

            std::vector<Unit*> units;
            std::vector<Structure*> structures;
            std::vector<Living*> all;
    };
}

