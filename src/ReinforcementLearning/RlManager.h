#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include <torch/cuda.h>
#include <torch/optim/adamw.h>
#include <torch/torch.h>

#include <deque>
#include <chrono>
#include <memory>
#include <variant>
#include <fstream>

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
    RlManager();
    void InitializeDQN(Map map, Player player, Player enemy);
    void StartPolicy(Map m, Player player, Player enemy);
    void AddMemory(const Transition& experience);
    size_t Size() const { return memory.size(); }
    void Serialize(const State& state);
    void Deserialize(const State& state);
    double CalculateStateReward(State state);

  private:
    State CreateCurrentState(Map map, Player player, Player enemy);
    Transition CreateTransition(State s, actionT a, State nextS);

  private: 
    std::deque<Transition> memory;
    DQN policy_net;
    DQN target_net;

    float gamma = 0.92f;
    bool calledMemOnce = false;
    const int batchSize = 32;
    const int maxSize = 10000;
};
#endif
