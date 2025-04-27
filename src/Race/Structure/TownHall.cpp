#include "TownHall.h"

TownHall::TownHall(Vec2 coord) {
    name = "Town Hall";
    description = "Can recruit peasants.";
    health = 1500;
    maxHealth = health;
    coordinate = coord;

    goldCost = 590;
    is = HALL;
    buildTime = 180;

    description = "Can build unit upgrades.";
}

TownHall::TownHall(Vec2 coord, float hp) {
    name = "Town Hall";
    description = "Can recruit peasants.";
    health = hp;
    maxHealth = 1500;
    coordinate = coord;
    goldCost = 590;
    is = HALL;
    buildTime = 180;
    description = "Can build unit upgrades.";
}
Structure *TownHall::Clone() const {
    return new TownHall(*this);
}
std::string TownHall::GetDescription() {
    return "Town hall.";
}
void TownHall::FinishBuilding() {
}

void TownHall::UpgradeEquipment(Upgrade &curr) {
}
