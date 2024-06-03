#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include <torch/cuda.h>
#include <torch/optim/adamw.h>
#include <torch/torch.h>

#include <deque>

#include "../Map/Map.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../State/Player.h"
#include "DQN.h"
#include "Transition.h"

class ReplayMemory {
  public:
    ReplayMemory(){}
    
    void InitializeDQN(Map map, Player &player, Player &enemy);
    void Push(Transition &t) {}
    void StartPolicy(Map m, Player& player, Player &enemy);

  private:
    State CreateMemoryState(Map map, Player &player, Player &enemy);

  private: 
    Transition transitions;
    DQN policy_net;
    DQN target_net;
    bool calledMemOnce = false;
};
// n
#endif
