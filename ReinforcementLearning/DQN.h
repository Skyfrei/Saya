#ifndef DQN_H
#define DQN_H
#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>

#include "../Race/Unit/Unit.h"
#include "ReplayMemory.h"

class DQN : public torch::nn::Module {
 public:
  DQN(int actionNumber);
  torch::nn::Linear Forward(torch::Tensor x);

 private:
  void Train();
  void SelectAction(State state);
  void OptimizeModel();

 private:
  float gamma = 0.92f;
  int episodeNumber = 50;
  int stepsDone = 0;

  torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};
};

#endif
