#include "DQN.h"
DQN::DQN(int actionNumber) {
  layer1 = register_module("layer1", torch::nn::Linear());
  layer2 = register_module("layer2", torch::nn::Linear(128, 128));
  layer3 = register_module("layer3", torch::nn::Linear(128, actionNumber));
}

torch::nn::Linear DQN::Forward(torch::Tensor x) {
  x = torch::relu(layer1(x));
  x = torch::relu(layer2(x));
  return layer3;
}

void DQN::SelectAction(State state) {}
