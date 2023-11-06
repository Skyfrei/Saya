#include "Structure.h"


namespace Warcraft::Structures
{

    class TownHall : public Structure
    {
        public:
            TownHall()
            {
                name = "Town Hall";
                description = "Can recruit peasants.";
                health = 1500;
                maxHealth = health;
                
                goldCold = 590;
                type = HALL;
                buildTime = 180;
            }
        public:
            
    }
}
