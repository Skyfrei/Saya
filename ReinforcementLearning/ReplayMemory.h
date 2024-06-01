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
  ReplayMemory() {
    torch::Device device(torch::kCPU);
    if (torch::cuda::is_available()) {
      device = torch::Device(torch::DeviceType::CUDA);
    }
    policy_net.to(device);
    target_net.to(device);

    torch::optim::AdamW optimizer(
        policy_net.parameters(),
        torch::optim::AdamWOptions(0.01).weight_decay(1e-4));
  }
  void CreateMemoryState(Map map, Player &player, Player &enemy);
  void Push(Transition &t) {}

  Transition transition;
  DQN policy_net;
  DQN target_net;
};
// n
#endif
