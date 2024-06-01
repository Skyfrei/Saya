#include "Reward.h"

float Reward::CalculateReward(const State& currentState, const actionT& action, const State& nextState) {
    float totalReward = 0.0f;

    if (std::holds_alternative<AttackAction>(action)) {
        if (IsUnitKilled(currentState, nextState)) {
            totalReward += RewardForEvent(KILL_UNIT);
        }
    } else if (std::holds_alternative<BuildAction>(action)) {
        if (IsStructureBuilt(currentState, nextState)) {
            totalReward += RewardForEvent(RECRUIT_SOLDIER);
        }
    } else if (std::holds_alternative<FarmGoldAction>(action)) {
        if (IsGoldFarmed(currentState, nextState)) {
            totalReward += RewardForEvent(FARM_GOLD);
        }
    }

    return totalReward;
}

float Reward::RewardForEvent(RewardEvent event) {
    switch (event) {
        case ATTACK_UNIT:
          return 0.35f;
        case DESTROY_HALL:
          return 1.0f;
        case FARM_GOLD:
          return 0.12f;
        case RECRUIT_SOLDIER:
          return 0.2f;
        case MOVE_UNIT:
          return 0.05f;
        case BUILD_STRUCTURE:
          return 0.10f;
        default:
            return 0.0f;
    }
}
