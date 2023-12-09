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
            void CastSpell(int s)
            {
                switch(s)
                {
                    case 0:
                        spells[0]->ProcEffect();
                        break;

                    case 1:
                        spells[1]->ProcEffect();
                        break;

                    case 2:
                        spells[2]->ProcEffect();
                        break;
                }
            }

            void RegenMana()
            {
                if (mana + manaRegen >= maxMana)
                    return;
                mana += manaRegen;
            }

            void LevelUp()
            {
                if (level >= 10)
                    return;
                strength += 2;
                health += 50;
                maxHealth += 50;
                hpRegen += 0.1;
                agility += 1;
                intelligence += 3;
                mana += 45;
                maxMana += 45;
                manaRegen += 0.15;
                attack += 3;
                attackCooldown -= 0.02;
            }

        public:
            bool hiredOnce = false;
            float strength;
            float agility;
            float intelligence;
            int8_t level = 1;


            std::vector<Spell*> spells;
            Attribute primaryAttribute;
        

    };
}

