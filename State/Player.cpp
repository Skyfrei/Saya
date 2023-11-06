#include "Player.h"

void Player::Initialize()
{
    units.push_back(new Peasant());
    units.push_back(new Peasant());
    units.push_back(new Peasant());
    units.push_back(new Peasant());
    units.push_back(new Peasant());

    structures.push_back(new TownHall());
    
}

void Player::ValidateFood()
{
    for(int i = 0; i < units.size(); i++)
    {
        food.x += units[i].foodCost;
    }
}

Upkeep Player::ValidateUpkeep()
{
    if (food.x <= 20)
        return Upkeep.NIE;
    
    else if (food.x <= 45)
        return Ukeep.LOW;

    return Upkeep.HIGH;
}

void Player::UpdateGold(int g)
{
    gold += g;
}

void Player::Move(std::vector<Unit*> uns)
{
    
}

void Player::IncreaseFoodCapac(Farm& f)
{
    if (food.y + f.GetFood() >= 60 )
        return 60;
        
    return food.y + f.GetFood();
}