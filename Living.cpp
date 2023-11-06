#include "Living.h"

void Living::RegenHealth()
{
    if (health + healthRegen >= maxHealth )
        return;
    health += healthRegen;
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