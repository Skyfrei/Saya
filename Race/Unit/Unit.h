#pragma once
#include <vector>
#include "../Spells/Spell.h"
#include "../../Living.h"
#include <string>
#include "../Tools/Vec2.h"

using namespace Warcraft::Spells;
using Warcraft::Living;

namespace Warcraft::Units
{
    enum Type
    {
        PEASANT,
        OTHER
    };

    enum Move
    {
        W,
        NW,
        N,
        NE,
        E,
        SE,
        S,
        SW
    };

    class Unit : public Living
    {
        public:
            Unit()
            {
                
            }
        
        public:
            void Move(Move dir);
            void Attack(Living& un);
            void GoTo(Vec2 coord);

        public:
        
            bool isInvisible = false;
            float attack;
            float attackCooldown;
            float attackRange;
            int movementSpeed = 1;
            float hpRegen = 0.25f;
            Type is;
    };
}
