#pragma once
#include <vector>
#include "../Spells/Spell.h"
#include "../../Living.h"
#include "../../Map/Terrain.h"
#include <string>
#include <queue>
#include "../../Tools/Vec2.h"

#define n 15

using namespace Warcraft::Spells;
using Warcraft::Living;
using namespace Warcraft::Environment;

namespace Warcraft::Units
{
    enum UnitType
    {
        FOOTMAN,
        PEASANT,
        SORCERESS,
        ARCHMAGE,
        BLOODMAGE,
        OTHER
    };

    enum MoveType
    {
        W,
        NW,
        N,
        NE,
        E,
        SE,
        S,
        SW,
        STAY
    };

    class Unit : public Living
    {
        public:
            Unit()
            {
                is = OTHER;
                attackCooldown = 1;

            }
            virtual ~Unit() = default;
        
        public:
            void Move(Vec2 terr)
            {
                Vec2 difference;
                difference.x = coordinate.x - terr.x;
                difference.y = coordinate.y - terr.y;

                if (difference.x > 0 && difference.y > 0)
                    ChangeCoordinate(NW);
                else if (difference.x > 0 && difference.y <0)
                    ChangeCoordinate(NE);
                else if (difference.x < 0 && difference.y > 0)
                    ChangeCoordinate(SW);
                else if (difference.x < 0 && difference.y < 0)
                    ChangeCoordinate(SE);
                else if (difference.x == 0 && difference.y < 0)
                    ChangeCoordinate(E);
                else if (difference.x == 0 && difference.y > 0)
                    ChangeCoordinate(W);
                else if (difference.x > 0 && difference.y == 0)
                    ChangeCoordinate(N);
                else if (difference.x < 0 && difference.y == 0)
                    ChangeCoordinate(S);
            }

            bool WithinDistance(Vec2 terr)
            {
                Vec2 difference;
                difference.x = coordinate.x - terr.x;
                difference.y = coordinate.y - terr.y;

                if (std::abs(difference.x) <= 1 && std::abs(difference.y) <= 1)
                    return true;

                return false;
            }

            void Attack(Living& un)
            {
                if (WithinDistance(un.coordinate))
                {
                    if (GetAttackTime() == true)
                        un.health -= attack;
                }
                else
                    Move(un.coordinate);
            }
            bool GetAttackTime()
            {
                time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::milli> diff = time - time1;
                time1 = std::chrono::high_resolution_clock::now();
                
                if (diff.count() >= attackCooldown)
                    return true;

                return false;
            }

            void RegenHealth()
            {
                time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::milli> diff = time - hpTime;
                hpTime = std::chrono::high_resolution_clock::now();

                if (health + hpRegen >= maxHealth)
                    return;
                if (diff.count() >= attackCooldown)
                    health += hpRegen;
            }
        private:
            void ChangeCoordinate(MoveType dir)
            {

                switch (dir)
                {
                    case W:
                        coordinate.y -= 1;
                        break;
                    case NW:
                        coordinate.x -= 1;
                        coordinate.y -= 1;
                        break;
                    case N:
                        coordinate.x -= 1;
                        break;
                    case NE:
                        coordinate.x -= 1;
                        coordinate.y += 1;
                        break;
                    case E:
                        coordinate.y += 1;
                        break;
                    case SE:
                        coordinate.x += 1;
                        coordinate.y += 1;
                        break;
                    case S:
                        coordinate.x += 1;
                        break;
                    case SW:
                        coordinate.x += 1;
                        coordinate.y -= 1;
                        break;
                    default:
                        break;
                }
            }

        public:
        
            bool isInvisible = false;
            float attack;
            float attackCooldown;
            float mana = 100;
            float manaRegen = 0.25;
            int maxMana = 100;
            float attackRange;
            int movementSpeed = 1;
            float hpRegen = 0.25f;
            UnitType is;
            std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point hpTime = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    };
}
