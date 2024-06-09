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
    
    void InitializeDQN(Map map, Player player, Player enemy);
    void StartPolicy(Map m, Player player, Player enemy);
    std::vector<Transition> Sample(size_t batch_size);
    size_t Size() const { return memory.size(); }

  private:
    State CreateCurrentState(Map map, Player player, Player enemy);
    Transition CreateTransition(State s, actionT a, NextState nextS);

  private: 
    std::deque<Transition> memory;
    DQN policy_net;
    DQN target_net;
    bool calledMemOnce = false;
};
// n
#endif
