//
// Created by Klavio Tarka on 14.12.23.
//
#include "Living.h"

namespace Warcraft
{
    bool Living::HasEnoughGold(int playerGold, int cost)
    {
        if (playerGold >= cost)
            return true;
        return false;
    }

    bool Living::Die()
    {
        if (health <= 0 )
            return true;
        return false;
    }
}
