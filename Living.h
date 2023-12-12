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
                //id = objNumber;
                //objNumber++;
            }
            virtual ~Living() = default;
            //Living::Living(const Living&)
            //Living& Living::operator=(const Living&)
            //Living::Living(Living&&)
            //Living& Living::operator=(Living&&)

        virtual std::string GetDescription() = 0;

        public:

            bool HasEnoughGold(int playerGold, int cost)
            {
                if (playerGold >= cost)
                    return true;
                return false;
            }

            bool Die()
            {
                if (health <= 0 )
                    return true;
                return false;
            }
            
        public:
            std::string name;
            std::string description;
            int health;
            //static int objNumber;
            int id;

            float maxHealth;

            int goldCost = 0;
            int foodCost = 0;
            
            int buildTime;

            Side belongs;

            Vec2 coordinate;

            
    };
}
