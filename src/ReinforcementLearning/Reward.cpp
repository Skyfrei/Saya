#include "Reward.h"

float GetRewardFromAction(actionT &action) {
    float reward = 0.0f;
    if (std::holds_alternative<MoveAction>(action))
    {
        reward += 0.1f;
    }
    else if (std::holds_alternative<AttackAction>(action))
    {
        const AttackAction &attackAction = std::get<AttackAction>(action);
        if (attackAction.object->health <= 0.0f)
        {
            reward += 0.5;
        }
        reward += 0.01;
    }
    else if (std::holds_alternative<BuildAction>(action))
    {
        // const BuildAction &buildAction = std::get<BuildAction>(action);
        // if (buildAction.stru->health >= buildAction.stru->maxHealth)
        //{
        //     reward += 0.3;
        // }
        reward += 0.3;
    }
    else if (std::holds_alternative<FarmGoldAction>(action))
    {
        reward += 0.5f;
    }
    else if (std::holds_alternative<RecruitAction>(action))
    {
        reward += 0.3f;
    }
    return reward;
}

float CalculateReward(const State &currentState, const actionT &action,
                      const State &nextState) {
    return 0.0f;
}
