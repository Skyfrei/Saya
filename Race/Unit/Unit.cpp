#include "Unit.h"

void Unit::RegenHealth()
{
    if (health + healthRegen >= maxHealth)
        return;
    health += healthRegen;
}

void Unit::GoTo(Vec2 coord)
{

}

void Unit::Attack(Living& un)
{
    un.health -= attack;
}

void Unit::Move(Move dir)
{
    switch (dir)
    {
        case W:
            coordinate.x -= 1;
            break;
        case NW:
            coordinate.x -= 1;
            coordinate.y += 1;
            break
        case N:
            coordinate.y += 1;
            break
        case NE:
            coordinate.x += 1;
            coordinate.y += 1;
            break
        case E:
            coordinate.x += 1;
            break
        case SE:
            coordinate.x += 1;
            coordinate.y -= 1;
            break
        case S:
            coordinate.y -= 1;
            break
        case SW:
            coordinate.x -= 1;
            coordinate.y -= 1;
            break;
    }
}