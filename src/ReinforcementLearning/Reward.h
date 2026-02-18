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
    }
    else if (std::holds_alternative<AttackAction>(action))
    {
        const AttackAction &attackAction = std::get<AttackAction>(action);
        if (attackAction.finished){
            reward += 5.0f;
        }else{
            if (attackAction.unit != nullptr &&
                attackAction.unit->coordinate == attackAction.object->coordinate) {
                float survivalPanic = (attackAction.unit->health / attackAction.unit->maxHealth);
                reward += (0.01f * attackAction.unit->attack) * survivalPanic;
            }
        }
    }
    else if (std::holds_alternative<BuildAction>(action))
    {
        const BuildAction &buildAction = std::get<BuildAction>(action);

        if (buildAction.finished){
             reward += 10.0f;
        }else{
            int gold = 0;
            if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
            if (gold < buildAction.stru->goldCost){
                reward -= 0.01f;
                return reward;
            }
            Peasant &p = static_cast<Peasant &>(*buildAction.peasant);
            if (p.coordinate == buildAction.coordinate &&
                buildAction.stru->health < buildAction.stru->maxHealth &&
                buildAction.stru->health > 0){
                reward += 0.1f;       
            }
        }
    }
    else if (std::holds_alternative<FarmGoldAction>(action))
    {
        int gold = 0;
        if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
        const FarmGoldAction &farmAction = std::get<FarmGoldAction>(action);
        Peasant &p = static_cast<Peasant &>(*farmAction.peasant);

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
            reward += 10 * ratio;  
        }else{
            if (p.coordinate == farmAction.terr->coord){
                reward += 0.1f;     
            }
        }
    }
    else if (std::holds_alternative<RecruitAction>(action))
    {
        const RecruitAction &recruitAction = std::get<RecruitAction>(action);
        int gold = 0;
        int food = 0;
        if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
        if constexpr (sizeof...(Args) >= 3) food = std::get<2>(arg_tuple);

        std::unique_ptr<Unit> unit;
        bool is_enough_resources = true;

        switch(recruitAction.unitType){
            case FOOTMAN:
                unit = std::make_unique<Footman>();
                if (gold < unit->goldCost) {
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    is_enough_resources = false;
                }
                if (!is_enough_resources){
                    reward -= 0.01f;
                    return reward;
                }
                break;

            case PEASANT:
                unit = std::make_unique<Peasant>();
                if (gold < unit->goldCost) {
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    is_enough_resources = false;
                }
                if (!is_enough_resources){
                    reward -= 0.01f;
                    return reward;
                }
                break;

            case ARCHMAGE:
                unit = std::make_unique<ArchMage>();
                if (gold < unit->goldCost) 
                    is_enough_resources = false;

                if (food < unit->foodCost)
                    is_enough_resources = false;

                if (!is_enough_resources){
                    reward -= 0.01f;
                    return reward;
                }
                reward += 0.2f;
                break;

            case BLOODMAGE:
                unit = std::make_unique<BloodMage>();
                if (gold < unit->goldCost)
                    is_enough_resources = false;

                if (food < unit->foodCost) 
                    is_enough_resources = false;

                if (!is_enough_resources){
                    reward -= 0.01f;
                    return reward;
                }
                reward += 0.2f;
                break;

            default:
                break;
        }
        reward += 5.0f;
    }
    else if (std::holds_alternative<EmptyAction>(action)){
        reward -= 1.0f;
    }
    return reward;
}

float CalculateReward(const State &currentState, const actionT &action,
                      const State &nextState);
#endif // REWARD_H
