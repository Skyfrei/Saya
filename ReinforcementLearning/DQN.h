#ifndef DQN_H
#define DQN_H
#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>
#include <vector>

#include "../Race/Unit/Unit.h"
#include <cmath>
#include <random>
#include <map>
#include "../Tools/Vec2.h"
#include "Transition.h"

#define MAX_UNITS 40
#define MAX_STRUCTS 30
#define PEASANT_INDEX_IN_UNITS 10
#define HALL_INDEX_IN_STRCTS 3
#define BARRACK_INDEX_IN_STRUCTS 5

class DQN : public torch::nn::Module {
 public:
  DQN(){}
  torch::Tensor Forward(torch::Tensor x);
  void Initialize(State state);
  actionT SelectAction(State state); // gotta return an action

 private:
  void OptimizeModel(std::deque<Transition> memory);
  torch::Tensor TurnStateInInput(State state);
  actionT MapIndexToAction(int actionIndex);


 private:
  float gamma = 0.92f;
  int episodeNumber = 50;
  int stepsDone = 0;
  float startEpsilon = 0.9f;
  float endEpsilon = 0.05f;
  float epsilonDecay = 1000.0f;
  float learningRate = 0.001f;
  int actionSize = 0;
  int inputSize = 0;
  const int batchSize = 32;
  
  int mapSize = MAP_SIZE * MAP_SIZE;
  int moveAction = mapSize + MAX_UNITS;
  int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
  int buildAction = attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * mapSize;
  int farmAction = buildAction + PEASANT_INDEX_IN_UNITS * mapSize * HALL_INDEX_IN_STRCTS; // town hall size multipled here as well
  int recruitAction = farmAction + 2 * NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size
  


  torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};
};

#endif
