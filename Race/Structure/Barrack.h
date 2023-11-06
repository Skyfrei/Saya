#pragme once
#include "Structure.h"
#include "../Unit/Unit.h"
#include "../Unit/Peasant.h"
#include "../Unit/Sorceress.h"
#include "../Unit/Footman.h"
#include "../Unit/Hero/Archmage.h"
#include "../Unit/Hero/BloodMage.h"

using namespace Warcraft::Units;
using namespace Warcraft::Units::Heroes;

namespace Warcraft::Structures
{
    enum UnitType
    {
        FOOTMAN,
        PEASANT,
        SORCERESS,
        ARCHMAGE,
        BLOODMAGE
    };

    class Barrack : public Structure
    {
        public:
            Barrack()
            {
                name = "Barrack";
                description = "Can create units.";
                health = 1500;

                goldCold = 220;

                buildTime = 70;
            }
        
        public:
            Unit& CreateUnit(UnitType type)
            {
                switch (type)
                {
                    case FOOTMAN:
                        Footman man;
                        return man;

                    case PEASANT:
                        Peasant slave;
                        return slave;

                    case SORCERESS:
                        Sorceress sorc;
                        return sorc;

                    case ARCHMAGE:
                        Archmage arch;
                        return arch;
                        
                    case BLOODMAGE:
                        BloodMage blood;
                        return blood;
                }
                return;
            }
    };
}
