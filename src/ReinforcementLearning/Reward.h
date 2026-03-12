#ifndef REWARD_H
#define REWARD_H

#include <tuple>
#include <cmath>

#include "Transition.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Unit/Hero/BloodMage.h"
#include "../Race/Unit/Hero/ArchMage.h"
#include "../Race/Structure/Structure.h"
#include <iostream>

template<typename... Args>
float GetRewardFromAction(Args&&... args) {
    float reward = 0.0f;
    auto arg_tuple = std::forward_as_tuple(std::forward<Args>(args)...);
    actionT&& action = std::get<0>(arg_tuple); 

    if (std::holds_alternative<MoveAction>(action))
    {
        const MoveAction &moveAction = std::get<MoveAction>(action);
        if (moveAction.destCoord.x >= MAP_SIZE ||
            moveAction.destCoord.y >= MAP_SIZE){
            reward -= 1.0f;
        }
        Peasant* p = dynamic_cast<Peasant*>(moveAction.unit);
        if (p && p->goldInventory > 0) {
            Vec2 currentDiff = p->coordinate - moveAction.destCoord;
            Vec2 prevDiff = moveAction.destCoord - moveAction.prevCoord;

            float oldDistSq = (prevDiff.x * prevDiff.x) + (prevDiff.y * prevDiff.y);
            float newDistSq = (currentDiff.x * currentDiff.x) + (currentDiff.y * currentDiff.y);

            if (newDistSq < oldDistSq) {
                reward += 0.01f; // Closer to base!
            } else {
                reward -= 0.1f; // Walking away/sideways!
            }
        }
    }
    else if (std::holds_alternative<AttackAction>(action))
    {
        const AttackAction &attackAction = std::get<AttackAction>(action);
        if (attackAction.object == nullptr){
            if (attackAction.finished){
                reward += 20.0f;
            }
        }else{
            TownHall* hall = dynamic_cast<TownHall*>(attackAction.object);
            if (hall)
                reward += 1.5f;
            else
                reward += 0.75f;
            if (attackAction.unit && attackAction.unit->coordinate == attackAction.object->coordinate) {
                reward += attackAction.unit->attack * 0.4f;
            }
        }
    }
    else if (std::holds_alternative<BuildAction>(action))
    {
        const BuildAction &buildAction = std::get<BuildAction>(action);
        if (!buildAction.stru)
            return reward;

        if (buildAction.finished && buildAction.stru->isBuilt){
            reward += 20.0f;
        }else{
            Peasant &p = static_cast<Peasant &>(*buildAction.peasant);
            if (buildAction.stru != nullptr){ 
                if (p.coordinate == buildAction.coordinate &&
                    buildAction.stru->health < buildAction.stru->maxHealth &&
                    buildAction.stru->health > 0){
                    reward += 0.1f;       
                }
            }
        }
    }
    else if (std::holds_alternative<FarmGoldAction>(action))
    {
        const FarmGoldAction &farmAction = std::get<FarmGoldAction>(action);
        Peasant &p = static_cast<Peasant &>(*farmAction.peasant);
        if (!farmAction.hall)
            return reward;

        if (farmAction.terr->resourceLeft <= 0) {
            reward -= 0.1f; 
            return reward;
        }
        if (p.goldInventory >= p.maxGoldInventory &&
            p.coordinate == farmAction.terr->coord) {
            reward -= 0.1f; 
            return reward;
        }

        if (farmAction.finished){
            float ratio = (float)farmAction.gold / p.maxGoldInventory;
            reward += 3 * ratio;  
        }else{
            if (p.coordinate == farmAction.terr->coord){
                reward += 0.1f;     
            }
        }
    }
    else if (std::holds_alternative<RecruitAction>(action))
    {
        const RecruitAction &recruitAction = std::get<RecruitAction>(action);
        if (!recruitAction.finished)
            return -0.1f;

        std::unique_ptr<Unit> unit;
        switch(recruitAction.unitType){
            case FOOTMAN:
                unit = std::make_unique<Footman>();
                reward += 3.0f * (unit->goldCost / 10.0f);
                break;

            case PEASANT:
                unit = std::make_unique<Peasant>();
                reward += 3.0f * (unit->goldCost / 10.0f);
                break;

            case ARCHMAGE:
                unit = std::make_unique<ArchMage>();
                reward += 3.0f * (unit->goldCost / 10.0f);
                break;

            case BLOODMAGE:
                unit = std::make_unique<BloodMage>();
                reward += 3.0f * (unit->goldCost / 10.0f);
                break;
            default:
                break;
        }
    }
    else if (std::holds_alternative<EmptyAction>(action)){
        reward -= 1.0f;
    }
    return reward;
}

float CalculateReward(const State &currentState, const actionT &action,
                      const State &nextState);
#endif // REWARD_H
