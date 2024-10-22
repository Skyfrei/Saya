#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include <torch/cuda.h>
#include <torch/optim/adamw.h>
#include <torch/torch.h>

#include <deque>
#include <chrono>
#include <memory>
#include <variant>

#include "../State/Map.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Peasant.h"
#include "../State/Player.h"
#include "../Race/Structure/TownHall.h"
#include "DQN.h"
#include "Transition.h"



class RlManager {
  public:
    RlManager(){}
    
    void InitializeDQN(Map map, Player player, Player enemy);
    void StartPolicy(Map m, Player player, Player enemy);
    std::vector<Transition> Sample(size_t batch_size);
    size_t Size() const { return memory.size(); }
    void OptimizeModel(std::deque<Transition> memory);
    void SaveModel();
    void LoadModel();

  private:
    State CreateCurrentState(Map map, Player player, Player enemy);
    Transition CreateTransition(State s, actionT a, NextState nextS);

  private: 
    std::deque<Transition> memory;
    DQN policy_net;
    DQN target_net;
    float gamma = 0.92f;
    bool calledMemOnce = false;
    const int batchSize = 32;
};
// n
#endif
