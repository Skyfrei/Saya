#pragma once

#include "../Unit.h"
#include "../../Spells/Spell.h"
#include <vector>
#include <chrono>

using namespace Warcraft::Spells;

namespace Warcraft::Units::Heroes
{
    enum Attribute
    {
        STRENGTH,
        AGILITY,
        INTELLIGENCE
    };

    class Hero : public Unit
    {
        public: 
            Hero()
            {

            }

        public:
            void RegenMana()
            {
                mana += manaRegen;
            }
            void CastSpell(int s)
            {
                switch(s)
                {
                    case 0:
                        spells[0].CastEffect();
                        break;

                    case 1:
                        spells[1].CastEffect();
                        break;

                    case 2:
                        spells[2].CastEffect();
                        break;
                }
            }

        public:
            bool hiredOnce = false;
            float strength;
            float agility;
            float intelligence;


            std::vector<Spell*> spells;
            Attribute primaryAttribute;
        

    };
}

