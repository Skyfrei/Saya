//
// Created by Klavio Tarka on 13.12.23.
//
#include "Barrack.h"

void Barrack::CreateUnit(std::vector<std::shared_ptr<Unit>>& units, int& playerGold, UnitType type)
{
    std::shared_ptr<Unit> unit;

    switch (type)
    {
        case FOOTMAN:
            unit = std::make_shared<Footman>();
            break;

        case ARCHMAGE:
            unit = std::make_shared<Archmage>();
            break;

        case PEASANT:
            unit =  std::make_shared<Peasant>();
            break;

        case BLOODMAGE:
            unit = std::make_shared<BloodMage>();
            break;
    }
    if (HasEnoughGold(playerGold, unit->goldCost))
    {
        //std::cout<<playerGold - unit->goldCost;
        playerGold -= unit->goldCost;
        units.emplace_back(std::move(unit));
    }
}