#ifndef RLMANAGER_H
#define RLMANAGER_H

#include <torch/torch.h>
#include <torch/cuda.h>

#include "../Map/Map.h"
#include "../State/Player.h"
#include "../Race/Unit/Unit.h"
#include "DQN.h"
#include "Transition.h"

class RlManager {
 public:
  RlManager(){    

    inputSize = 2 + 4 + 6;
    torch::Device device(torch::kCPU);
    if (torch::cuda::is_available()) {
        device = torch::Device(torch::DeviceType::CUDA);
    }
    DQN policy_net(inputSize, actionNumber);
    policy_net.to(device);

    DQN target_net(inputSize, actionNumber);
    target_net.to(device); 
  }

  void StartPolicy(Map m, Player &player, Player &enemy);
  
  private:
    ReplayMemory memory;
    DQN policy_net;
    DQN target_net;
    int inputSize = 0;
    int actionNumber = NR_OF_ACTIONS;
};

#endif
