#include "Spell.h"



// Channel Time	Duration	Cooldown	Mana Cost	Range	Area of Effect	Allowed Targets
// 0.9 sec.	6 sec.	10 sec.	135	80	20	Ground, Enemy, Neutral, Friend, Building, Self, Tree

// Level	Effect	Hero Level Req
// 1	45 Pillar damage/sec. for 2.67 seconds, 4 Fire damage for 6 sec.	1
// 2	80 Pillar damage/sec. for 2.67 seconds, 8 Fire damage for 6 sec.	3
// 3	110 Pillar damage/sec. for 2.67 seconds, 12 Fire damage for 6 sec.

using namespace Warcraft::Spells;
class FlameStrike : public Spell
{
    public:
        FlameStrike()
        {
            cooldown = 10.0f;
            manaCost = 135;
            range = 80;
            areaOfEffect = 20;
            duration = 6.0f;
            name = "Flame Strike";
            type = MANUAL;
        }
        
    public:
        virtual void GetDescription(){}
        void ProcEffect() override {}
    
    public:
        float delay = 0.9f;
};