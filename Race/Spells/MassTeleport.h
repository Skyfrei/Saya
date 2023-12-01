#pragma once
#include "Spell.h"


//  Cooldown	Mana Cost	Casting Delay	Range	Area of Effect	Allowed Targets	Effect	Hero Level Req
//  30 sec.	100	3 sec.	Unlimited	80	Friendly Ground Units, Friendly Buildings	Teleports up to 24 units	6



// Teleports 24 of the player's nearby units, including the Archmage, to a friendly ground unit or structure.

using namespace Warcraft::Spells;
class MassTeleport : public Spell
{
    public:
        MassTeleport()
        {
            cooldown = 30.0f;
            manaCost = 100;
            range = 80;
            areaOfEffect = 80;
            duration = 6.0f;
            name = "Blizzard";
            type = MANUAL;

        }
        
    public:
        virtual void GetDescription(){}
        void ProcEffect() override {}
    
    public:
        float delay = 3.0f;
};