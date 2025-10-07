//
// Created by Klavio Tarka on 13.12.23.
//
#include "Barrack.h"

Barrack::Barrack(Vec2 coord) {
    health = 1500;
    maxHealth = health;
    goldCost = 220;
    is = BARRACK;
    buildTime = 70;
    coordinate = coord;
}

Barrack::Barrack(Vec2 coord, float hp) {
    coordinate = coord;
    health = hp;
    maxHealth = 1500;
    goldCost = 220;
    is = BARRACK;
    buildTime = 70;
}

void Barrack::CreateUnit(std::vector<std::unique_ptr<Unit>> &units, int &playerGold,
                         UnitType type) {
    std::unique_ptr<Unit> unit;

    switch (type)
    {
    case FOOTMAN:
        unit = std::make_unique<Footman>();
        break;

    case ARCHMAGE:
        unit = std::make_unique<ArchMage>();
        break;

    case PEASANT:
        unit = std::make_unique<Peasant>();
        break;

    case BLOODMAGE:
        unit = std::make_unique<BloodMage>();
        break;
    default:
        unit = std::make_unique<Peasant>();
        break;
    }
    if (HasEnoughGold(playerGold, unit->goldCost))
    {
        playerGold -= unit->goldCost;
        units.emplace_back(std::move(unit));
    }
}
