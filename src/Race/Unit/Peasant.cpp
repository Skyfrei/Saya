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

Peasant::Peasant() {
    health = 240;
    maxHealth = health;
    attack = 5.5;
    maxMana = 200.0f;
    mana = 200.0f;
    manaRegen = 0.67f;
    goldCost = 55;
    foodCost = 1;
    buildTime = 15;
    is = PEASANT;
}

Peasant::Peasant(Vec2 coord, float hp, float man) {
    coordinate = coord;
    health = hp;
    mana = man;
    maxHealth = 240;
    attack = 5.5;
    manaRegen = 0.67f;
    maxMana = 200.0f;

    goldCost = 55;
    foodCost = 1;
    buildTime = 15;
    is = PEASANT;
}

void Peasant::Build(Structure *str) {
    if (WithinDistance(str->coordinate))
    {
        if (CanAttack())
        {
            if (str->health + attack <= str->maxHealth)
            {
                str->health += attack;
            }else{
                str->health = str->maxHealth;
            }
        }
    }
    else{
        MoveAction mov(this, str->coordinate);
        InsertFrontAction(mov);
    }
}

void Peasant::FarmGold(Terrain &terr, TownHall &hall, int &g) {
    if (goldInventory >= maxGoldInventory) {
        if (WithinDistance(hall.coordinate)) {
            auto it = std::find_if(actionQueue.begin(), actionQueue.end(), [](actionT& act) {
                return std::holds_alternative<FarmGoldAction>(act);
            });

            if (it != actionQueue.end()) {
                FarmGoldAction& farmAct = std::get<FarmGoldAction>(*it);
                farmAct.gold = goldInventory; 
                goldInventory = 0;
                farmAct.finished = true;
            }
            return; 
        } else {
            MoveAction mov(this, hall.coordinate);
            InsertFrontAction(mov);
            return;
        }
    }
    else if (WithinDistance(terr.coord)){
        if (terr.type == GOLD && terr.resourceLeft > 0)
        {
            if (CanAttack())
            {
                goldInventory++;
                terr.resourceLeft--;
            }
        }else{
            for (auto& act : actionQueue) {
                if (auto* farmAct = std::get_if<FarmGoldAction>(&act)) 
                {
                    farmAct->finished = true;
                    farmAct->gold = goldInventory; 
                }
            }
        }
    }else
    {
        MoveAction mov(this, terr.coord);
        InsertFrontAction(mov);
    }
}
