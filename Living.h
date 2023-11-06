#pragma once

#include <iostream>
#include <string>
#include "Tools/Vec2.h"

namespace Warcraft
{   

    enum Side
    {
        PLAYER,
        ENEMY
    };

    class Living
    {
        public:
            Living()
            {
                id = objNumber;
                objNumber++;
            }

        virtual std::string GetDescription() = 0;

        public:
            void RegenHealth();

            bool IsDead();

            void Die();
            
        public:
            std::string name;
            std::string description;
            int health;
            static int objNumber;
            int id;
            
            float healthRegen = 0.25;
            float mana = 0;
            float maxMana;
            float maxHealth;
            float manaRegen = 1.15;

            int goldCold;
            int foodCost = 0;
            
            int buildTime;

            Side belongs;

            Vec2 coordinate;

            
    };
}
