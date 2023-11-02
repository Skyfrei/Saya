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

    enum ProducedAt
    {
        
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
            std::string name;
            std::string description;
            int health;
            int armor;
            static int objNumber;
            int id;
            
            float hpRegen = 0;
            float mana = 0;
            float manaRegen = 0;

            int goldCold;
            int foodCost = 0;
            
            int buildTime;

            Side belongs;
            ProducedAt buildsAt;

            Vec2 coordinate;

            
    };
}
