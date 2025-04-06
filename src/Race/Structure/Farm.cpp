#include "Farm.h"
Farm::Farm(Vec2 coord) {
    name = "Farm";
    description = "Gives 6 food.";
    health = 500;
    maxHealth = health;
    goldCost = 100;
    buildTime = 10;
    is = FARM;
    coordinate = coord;
}

Farm::Farm(Vec2 coord, float hp) {
    name = "Farm";
    description = "Gives 6 food.";
    health = hp;
    maxHealth = 500;
    goldCost = 100;
    buildTime = 10;
    is = FARM;
    coordinate = coord;
}

std::string Farm::GetDescription() {
    return description;
}
void Farm::FinishBuilding() {
}

std::unique_ptr<Structure> Farm::Clone() const {
    return std::make_unique<Farm>(*this);
}
int Farm::GetFood() {
    return 5;
}
