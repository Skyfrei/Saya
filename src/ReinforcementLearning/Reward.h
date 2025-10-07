#ifndef REWARD_H
#define REWARD_H

#include "Transition.h"
#include <tuple>
#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Structure.h"

template<typename... Args>
float GetRewardFromAction(Args... args) {
    float reward = 0.0f;
    auto arg_tuple = make_tuple(args...);
    actionT action = std::get<0>(arg_tuple);

    if (std::holds_alternative<MoveAction>(action))
    {
        reward += 0.1f;
    }
    else if (std::holds_alternative<AttackAction>(action))
    {
        // use unit here to check if death or not
        const AttackAction &attackAction = std::get<AttackAction>(action);
        if (attackAction.object->health <= 0.0f)
        {
            reward += 0.5;
        }
        reward += 0.01;
    }
    else if (std::holds_alternative<BuildAction>(action))
    {
        const BuildAction &buildAction = std::get<BuildAction>(action);
        // if (buildAction.stru->health >= buildAction.stru->maxHealth)
        //{
        //     reward += 0.3;
        // }
        int gold = 0;
        if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
        if (gold < buildAction.stru->goldCost){
            reward -= 0.3;
            return reward;
        }
        reward += 0.3;
    }
    else if (std::holds_alternative<FarmGoldAction>(action))
    {
        int gold = 0;
        if constexpr (sizeof...(Args) >= 2) gold = std::get<1>(arg_tuple);
        if (gold <= 500) {
            reward += 0.5f;  
        }
        else if (gold > 500) { /*gradual drop*/
            reward += 0.5f - std::log(gold / 500) / std::log(16); 
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
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;

            case PEASANT:
                unit = std::make_unique<Peasant>();
                if (gold < unit->goldCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;

            case ARCHMAGE:
                unit = std::make_unique<ArchMage>();
                if (gold < unit->goldCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                reward += 0.2f;

            case BLOODMAGE:
                unit = std::make_unique<BloodMage>();
                if (gold < unit->goldCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (food < unit->foodCost) {
                    reward -= 0.3;
                    is_enough_resources = false;
                }
                if (!is_enough_resources)
                    return reward;
                reward += 0.2f;

            default:
                break;
        }
        reward += 0.3f;
    }
    return reward;
}

float CalculateReward(const State &currentState, const actionT &action,
                      const State &nextState);
#endif // REWARD_H
