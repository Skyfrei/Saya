#include "Footman.h"

Footman::Footman() {
    name = "Footman";
    description = "A normal foot soldier.";
    health = 420;
    maxHealth = health;
    attack = 12.5;

    goldCost = 135;
    foodCost = 2;

    buildTime = 20;

    is = FOOTMAN;
}

Footman::Footman(Vec2 coord, float hp, float man) {
    name = "Footman";
    description = "A normal foot soldier.";
    maxHealth = 420;
    attack = 12.5;
    goldCost = 135;
    foodCost = 2;
    buildTime = 20;

    coordinate = coord;
    health = hp;
    mana = man;
    is = FOOTMAN;
}

std::string Footman::GetDescription() {
    return "Footman.";
}

std::unique_ptr<Unit> Footman::Clone() const {
    return std::make_unique<Footman>(*this);
}
