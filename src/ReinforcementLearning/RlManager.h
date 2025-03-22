#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include "../State/Map.h"
#include "../State/Player.h"
#include "DQN.h"
#include "Transition.h"

class RlManager {
  public:
    RlManager();
    void InitializeDQN(Map map, Player player, Player enemy);
    void StartPolicy(Map m, Player player, Player enemy);
    double CalculateStateReward(State state);

  private:
    State CreateCurrentState(Map map, Player player, Player enemy);
    Transition CreateTransition(State s, actionT a, State nextS);

  private: 
    DQN policy_net;
    DQN target_net;

    float gamma = 0.92f;
    bool calledMemOnce = false;
    const int batchSize = 32;
    const int maxSize = 10000;
};
#endif
