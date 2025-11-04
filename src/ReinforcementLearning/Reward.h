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
        reward -= 0.02f;
    }
    else if (std::holds_alternative<AttackAction>(action))
    {
        const AttackAction &attackAction = std::get<AttackAction>(action);
        if (attackAction.object->health <= 0.0f && attackAction.finished){
            reward += 0.5;
        }else{
            if (attackAction.unit != nullptr)
                reward += 0.001 * attackAction.unit->attack;
        }
    }
    else if (std::holds_alternative<BuildAction>(action))
    {
        const BuildAction &buildAction = std::get<BuildAction>(action);
        if (buildAction.stru->health >= buildAction.stru->maxHealth && buildAction.finished){
             reward += 0.3f;
        }else{
            int gold = 0;
            if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
            if (gold < buildAction.stru->goldCost){
                reward -= 0.1f;
                return reward;
            }
            reward += 0.10f;       
        }
    }
    else if (std::holds_alternative<FarmGoldAction>(action))
    {
        int gold = 0;
        if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
        const FarmGoldAction &farmAction = std::get<FarmGoldAction>(action);
        if (farmAction.finished){
            Peasant &p = static_cast<Peasant &>(*farmAction.peasant);
            float ratio = (float)farmAction.gold / p.maxGoldInventory;
            if (gold <= 500) {
                reward += 0.3f * ratio;  
            }
            else if (gold > 500) { /*gradual drop*/
                reward += 0.3f * ratio - std::log(gold / 500) / std::log(16); 
            }
        }else{
            if (gold <= 500) {
                reward += 0.02f;  
            }
            else if (gold > 500) { /*gradual drop*/
                reward += 0.02f - std::log(gold / 500) / std::log(16); 
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
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                break;

            case PEASANT:
                unit = std::make_unique<Peasant>();
                if (gold < unit->goldCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                break;

            case ARCHMAGE:
                unit = std::make_unique<ArchMage>();
                if (gold < unit->goldCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                reward += 0.2f;
                break;

            case BLOODMAGE:
                unit = std::make_unique<BloodMage>();
                if (gold < unit->goldCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.1;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                reward += 0.2f;
                break;

            default:
                break;
        }
        reward += 0.3f;
    }
    else if (std::holds_alternative<EmptyAction>(action)){
        reward -= 0.02f;
    }
    return reward;
}

float CalculateReward(const State &currentState, const actionT &action,
                      const State &nextState);
#endif // REWARD_H
