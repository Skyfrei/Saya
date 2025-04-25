#ifndef REWARD_H
#define REWARD_H

#include "Transition.h"

float CalculateReward(const State &currentState, const actionT &action, const State &nextState);
float GetRewardFromAction(actionT& action);


#endif // REWARD_H
