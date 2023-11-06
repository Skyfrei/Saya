#include "Living.h"

bool Living::IsEnoughGold(int playerGold, int cost);
{
    if (playerGold >= cost)
        return true;
    
    return false;
}

bool Living::IsDead()
{
     if (health <= 0)
        return true;
    return false;
}

void Living::Die()
{
    
}