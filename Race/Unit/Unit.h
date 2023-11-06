#pragma once
#include <vector>
#include "../Spells/Spell.h"
#include "../../Living.h"
#include <string>

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
            void Move(Move dir)
            {
                switch (dir)
                {
                case W:
                    coordinate.x -= 1;
                    break;
                case NW:
                    coordinate.x -= 1;
                    coordinate.y += 1;
                    break;

                case N:
                    coordinate.y += 1;
                    break;

                case NE:
                    coordinate.x += 1;
                    coordinate.y += 1;
                    break;

                case E:
                    coordinate.x += 1;
                    break;

                case SE:
                    coordinate.x += 1;
                    coordinate.y -= 1;
                    break;

                case S:
                    coordinate.y -= 1;
                    break;

                case SW:
                    coordinate.x -= 1;
                    coordinate.y -= 1;
                    break;
                }
            }

            void Attack(Living& un)
            {
                un.health -= attack;
            }

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
