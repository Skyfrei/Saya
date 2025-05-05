#include "Farm.h"
Farm::Farm(Vec2 coord)
{
    health = 500;
    maxHealth = health;
    goldCost = 100;
    buildTime = 10;
    is = FARM;
    coordinate = coord;
}

Farm::Farm(Vec2 coord, float hp)
{
    health = hp;
    maxHealth = 500;
    goldCost = 100;
    buildTime = 10;
    is = FARM;
    coordinate = coord;
}

std::string Farm::GetDescription()
{
    return "Get 6 food";
}
void Farm::FinishBuilding()
{
}

Structure *Farm::Clone() const
{
    return new Farm(*this);
}
int Farm::GetFood()
{
    return 5;
}
