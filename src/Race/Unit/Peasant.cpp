//
// Created by Klavio Tarka on 13.12.23.
//
#include "Peasant.h"
#include "../../State/Map.h"
#include "../../State/Terrain.h"
#include "../Structure/Barrack.h"
#include "../Structure/Farm.h"
#include "../Structure/Structure.h"
#include "../Structure/TownHall.h"

Peasant::Peasant()
{
    health = 240;
    maxHealth = health;
    attack = 5.5;
    maxMana = 200.0f;
    mana = 200.0f;
    manaRegen = 0.67f;
    goldCost = 75;
    foodCost = 1;
    buildTime = 15;
    is = PEASANT;
}

Peasant::Peasant(Vec2 coord, float hp, float man)
{
    coordinate = coord;
    health = hp;
    mana = man;
    maxHealth = 240;
    attack = 5.5;
    manaRegen = 0.67f;
    maxMana = 200.0f;

    goldCost = 75;
    foodCost = 1;
    buildTime = 15;
    is = PEASANT;
}

void Peasant::Build(Structure *str)
{
    if (WithinDistance(str->coordinate))
    {
        if (CanAttack())
        {
            if (str->health + attack <= maxHealth)
            {
                str->health += attack;
            }
        }
    }
    else
        Move(str->coordinate);
}

void Peasant::FarmGold(Terrain &terr, TownHall &hall, int &g)
{
    if (goldInventory > 0 && WithinDistance(hall.coordinate))
    {
        goldInventory = 0;
    }
    if (goldInventory >= maxGoldInventory)
    {
        Move(hall.coordinate);
        return;
    }
    if (WithinDistance(terr.coord))
    {
        if (terr.type == GOLD)
        {
            if (CanAttack())
            {
                goldInventory++;
                terr.resourceLeft--;
                g++;
            }
        }
    }
    else
    {
        Move(terr.coord);
    }
}
