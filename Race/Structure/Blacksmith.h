#include "Structure.h"

namespace Warcraft::Structures
{
    enum Upgrade
    {
        WOOD,
        BRONZE,
        IRON,
        STEEL,
        MITHRIL
    };

    class Blacksmith : public Structure
    {
        public:
            Blacksmith()
            {
                name = "Blacksmith";
                description = "Can build unit upgrades.";
                health = 1200;

                goldCold = 200;

                buildTime = 50;
            }
        public:
            Upgrade currentUpgrade = WOOD;
            void UpgradeEquipment(Upgrade& curr)
            {
                
            };
            
    }
}
