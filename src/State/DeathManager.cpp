#include "DeathManager.h"

DeathManager* DeathManager::instance = nullptr;

void DeathManager::Init(Player* pl, Player* en){
    if (instance == nullptr){
        instance = new DeathManager(pl, en);
    }
}

DeathManager& DeathManager::GetSingleton(){
    return *instance;
}

void DeathManager::RemoveFromAttackAction(Living* deadEntity, Side side){
    if (deadEntity == nullptr) return;
    
    if (side == PLAYER){
        for (auto& unit : en->units) {
            for (auto& variantAct : unit->actionQueue) {
                if (std::holds_alternative<AttackAction>(variantAct)) {
                    auto& attack = std::get<AttackAction>(variantAct);
                    if (attack.object == deadEntity) {
                        attack.object = nullptr;
                        attack.finished = true;
                    }
                }
            }
        }
    }
    else{
        for (auto& unit : pl->units) {
            for (auto& variantAct : unit->actionQueue) {
                if (std::holds_alternative<AttackAction>(variantAct)) {
                    auto& attack = std::get<AttackAction>(variantAct);
                    if (attack.object == deadEntity) {
                        attack.object = nullptr;
                        attack.finished = true;
                    }
                }
            }
        }
    }
}
