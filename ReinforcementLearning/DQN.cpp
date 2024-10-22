#include "DQN.h"

void DQN::Initialize(State state){
  TensorStruct ts(state);
  inputSize = ts.GetTensor().size(1);
  actionSize = recruitAction;
  layer1 = register_module("layer1", torch::nn::Linear(inputSize, 128));
  layer2 = register_module("layer2", torch::nn::Linear(128, 128));
  layer3 = register_module("layer3", torch::nn::Linear(128, actionSize));

}

torch::Tensor DQN::Forward(torch::Tensor x) {
  x = torch::relu(layer1->forward(x));
  x = torch::relu(layer2->forward(x));
  x = layer3->forward(x);  
  return x;
}
// mapSize = 3 * 3 = 9
// MAP_SIZE = 3
actionT DQN::MapIndexToAction(int actionIndex) {

  if (actionIndex < moveAction ) {
    int col = actionIndex % MAP_SIZE;
    int row = (actionIndex / MAP_SIZE) % MAP_SIZE;
    int unitIndex = (actionIndex / (mapSize)) % MAX_UNITS;
    // TODO
  }
  else if(actionIndex < attackAction){
    int offset = actionIndex - moveAction;
    int playerUnit = (offset / (MAX_STRUCTS + MAX_UNITS)) % MAX_UNITS;
    int targetIndex = offset % (MAX_STRUCTS + MAX_UNITS);
    if (targetIndex < MAX_STRUCTS - 1){
      // pass enemy struct
    }else{
      int temp = targetIndex - MAX_STRUCTS;
      // pass enemy unit
    }
  }
  else if (actionIndex < buildAction){
    int offset = actionIndex - attackAction;
    int unit = offset / (NR_OF_STRUCTS * mapSize);
    int struSelect = (offset / mapSize) % NR_OF_STRUCTS;
    int mapSelect = offset % mapSize;
    int col = mapSelect % MAP_SIZE;
    int row = mapSelect / MAP_SIZE;
    // Pass build action here.
  }
  else if (actionIndex < farmAction){
    int offset = actionIndex - buildAction;
    int peasantIndex = offset / (mapSize * HALL_INDEX_IN_STRCTS);
    int hallIndex = (offset / mapSize) % HALL_INDEX_IN_STRCTS;
    int mapSelect = offset % mapSize;
    int col = mapSelect % MAP_SIZE;
    int row = mapSelect / MAP_SIZE;
    // pass farm action here
  }
  else if (actionIndex < recruitAction){
    int offset = actionIndex - farmAction;
    int unitType = offset / BARRACK_INDEX_IN_STRUCTS;
    int barrackIndex = offset % BARRACK_INDEX_IN_STRUCTS;
    // pass recruitAction here
  }

  return std::monostate();
}

actionT DQN::SelectAction(State state) { 
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist1(0.0, 1.0);

  float epsilonThreshold = endEpsilon + (startEpsilon - endEpsilon) * std::exp(-1. * stepsDone / epsilonDecay);
  stepsDone++;  
  float sample = dist1(rng);

  if (sample > epsilonThreshold) {
    at::Tensor action = std::get<1>(Forward(TurnStateInInput(state)).max(1)).view({1, 1});
    actionT result = MapIndexToAction(action.item<int>());
    return result;
  } 
  else {
    std::uniform_int_distribution<int> dist2(0, actionSize - 1);  // Distribution for action selection
    int actionIndex = dist2(rng);
    actionT result = MapIndexToAction(actionIndex);
    return result;
  }
}

torch::Tensor DQN::TurnStateInInput(State state){
  TensorStruct ts(state);
  return ts.GetTensor();
}


void DQN::PrintWeight(){
    std::cout<<this->layer1->weight[0][0]<<std::endl;
}
