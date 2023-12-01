#pragma once
#include "Spell.h"


// Range	Allowed Targets	Effect
// 80	Air, Ground, Organic, Self	Unit turned Ethereal with 50% movement speed.
// Level	Duration (Hero)	Mana Cost	Hero Level Req
// 1	12 (4) sec.	75	1
// 2	15 (5) sec.	60	3
// 3	18 (6) sec.

using namespace Warcraft::Spells;
class Banish : public Spell
{
    public:
        Banish()
        {
            cooldown = 6.0f;
            manaCost = 75;
            range = 60;
            areaOfEffect = 20;
            duration = 12.0f;
            name = "Banish";
            type = MANUAL;
        }
        
    public:
        void GetDescription() override{}
        void ProcEffect() override {}
    
    public:
        float delay = 0.9f;
};