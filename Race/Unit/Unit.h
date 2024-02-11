#pragma once
#include <vector>
#include "../Spells/Spell.h"
#include "../../Living.h"
#include "../../Map/Terrain.h"
#include <string>
#include "../../Tools/Vec2.h"
#include "../Structure/Structure.h"
#include "../../State/Task.h"
#include <chrono>
#include <unordered_map>

#define MAP_SIZE 15

namespace Warcraft::Units
{

    using namespace Warcraft::Spells;
    using Warcraft::Living;
    using namespace Warcraft::Environment;
    using namespace Warcraft::Structures;
    using namespace Warcraft::State;

    enum UnitType
    {
        FOOTMAN,
        PEASANT,
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
            Unit();
            virtual ~Unit() = default;
        
        public:
            void Move(Vec2 terr);
            bool WithinDistance(Vec2 terr);
            Vec2 FindDifference(Vec2 terr);
            void Attack(Living& un);
            bool GetAttackTime();
            void RegenHealth();
            void MoveDij(Vec2 terr);
            //void CheckTask(Task<Unit>& t);
            void ChangeCoordinate(MoveType dir);

        public:
        
            bool isInvisible = false;
            float attack{};
            float attackCooldown;
            float mana = 100;
            float manaRegen = 0.25;
            int maxMana = 100;
            float attackRange{};
            int movementSpeed = 1;
            float hpRegen = 0.25f;
            UnitType is;
            std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point hpTime = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    };
}
