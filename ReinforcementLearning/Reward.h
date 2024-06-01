#ifndef REWARD_H
#define REWARD_H

#include "Transition.h"
#include "Actions.h"  // Include necessary headers for Action variants

enum RewardEvent { ATTACK_UNIT, DESTROY_HALL, FARM_GOLD, RECRUIT_SOLDIER, MOVE_UNIT, BUILD_STRUCTURE };

class Reward {
public:
    static float CalculateReward(const State& currentState, const actionT& action, const State& nextState);

private:
    static float RewardForEvent(RewardEvent event);
};

#endif // REWARD_H
