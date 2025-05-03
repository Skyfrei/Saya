#include "Footman.h"

Footman::Footman() {
    health = 420;
    maxHealth = health;
    attack = 12.5;
    maxMana = 200.0f;
    mana = 200.0f;
    goldCost = 135;
    foodCost = 2;
    buildTime = 20;
    is = FOOTMAN;
}

Footman::Footman(Vec2 coord, float hp, float man) {
    maxHealth = 420;
    attack = 12.5;
    goldCost = 135;
    foodCost = 2;
    buildTime = 20;
    coordinate = coord;
    maxMana = 200.0f;
    health = hp;
    mana = man;
    is = FOOTMAN;
}

std::string Footman::GetDescription() {
    return "Footman.";
}

Unit *Footman::Clone() const {
    return new Footman(*this);
}
