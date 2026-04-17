#include "TownHall.h"

TownHall::TownHall(Vec2 coord) {
    health = 1500;
    maxHealth = health;
    coordinate = coord;
    goldCost = 590;
    is = HALL;
    buildTime = 180;
    attackTime = std::chrono::high_resolution_clock::now();
    attackCooldown = std::chrono::milliseconds(1000);
}

TownHall::TownHall(Vec2 coord, float hp) {
    health = hp;
    maxHealth = 1500;
    coordinate = coord;
    goldCost = 590;
    is = HALL;
    buildTime = 180;
    attackTime = std::chrono::high_resolution_clock::now();
    attackCooldown = std::chrono::milliseconds(1000);
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

bool TownHall::CanAttack() {
    auto currentCd = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - attackTime);
        
    if (currentCd >= attackCooldown) {
        attackTime = std::chrono::high_resolution_clock::now(); // Reset the timer
        return true;
    }
    return false;
}
