#pragma once
#include "Spell.h"


// Duration	Cooldown	Mana Cost	Range	Area of Effect	Allowed Targets
// 6 sec.	6 sec.	10	60	80	Air, Ground, Enemy, Friend, Organic, Neutral
// Level	Effect	Hero Level Req
// 1	15 Mana drained/sec., 30 Mana transferred/sec.	1
// 2	25 Mana drained/sec., 60 Mana transferred/sec.	3
// 3	40 Mana drained/sec., 90 Mana transferred/sec.	5

using namespace Warcraft::Spells;
class SiphonMana : public Spell
{
    public:
        SiphonMana()
        {
            cooldown = 6.0f;
            manaCost = 10;
            range = 60;
            areaOfEffect = 20;
            duration = 6.0f;
            name = "Siphon Mana";
            type = MANUAL;
        }
        
    public:
        virtual void GetDescription(){}
        void ProcEffect() override {}
    
    public:
        float delay = 0.9f;
};