#include "DQN.h"
#include <fstream>
#include <cmath>
#include <random>
#include <map>
#include "../Tools/Binary.h"

int mapSize = MAP_SIZE * MAP_SIZE;
int moveAction = mapSize * MAX_UNITS;
int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
int buildAction = attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * mapSize;
int farmAction = buildAction + PEASANT_INDEX_IN_UNITS * mapSize * HALL_INDEX_IN_STRCTS; // town hall size multipled here as well
int recruitAction = farmAction + 2 * NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size

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

void DQN::AddExperience(Transition trans){
    if(memory.size() >= memory_size){
        memory.pop_back();
    }
    memory.push_back(trans);
    
}
void DQN::SaveMemory(){
    std::string data_to_save = "";
    for (int i = 0; i < memory.size(); i++){
        data_to_save += memory[i].Serialize() + "\n";
    }
    std::ofstream file;
    file.open(memory_file);
    file<<data_to_save;
    file.close();
}

void DQN::LoadMemory(){
    std::ifstream file(memory_file);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open()){
        std::cout<< "File of experience couldn't be opened.";
        return;
    }
    while (std::getline(file, line)) {
        Transition trans;
        trans = trans.Deserialize(line);
       // trans = trans.Deserialize(line);
       // memory.push_back(trans);
    }
    file.close();
}

void DQN::SaveMemoryAsBinary(){
    std::vector<binary> data_to_save; 
    std::ofstream file;
    file.open("binary.bin", std::ios::binary);
        
    for (int i = 0; i < memory.size(); i++){
        std::vector<binary> data = memory[i].SerializeBinary();
        file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(binary));
    }
    
    file.close();
}
// 0-11 first bytes, 
void DQN::LoadMemoryAsBinary(){
    std::ifstream file("binary.bin", std::ios::binary);
    std::vector<binary> binaryData;
    int expectedBytes = 0;
    binary temp;

    if (!file.is_open()){
        std::cout<< "File of experience couldn't be opened.";
        return;
    }
    while (file.read(reinterpret_cast<char*>(&temp), sizeof(binary))) {
        if (binaryData.size() == 0){
            expectedBytes = std::get<int>(temp);
            std::cout<<expectedBytes<< " " ;
        }
        binaryData.push_back(temp);
        
        if (binaryData.size()  == expectedBytes + 1)
            binaryData.clear();


        //Transition trans;
        //trans = trans.DeserializeBinary(transitionData);
        //memory.push_back(trans);
    }
    file.close();
}

void DQN::Train(){
    torch::optim::SGD optimizer(this->parameters(), learningRate);
    float loss = 0.0f;
    
    for (int i = 0; i < epochNumber; i++){
        
    }
}

void DQN::Test(){

}
// mapSize = 3 * 3 = 9
// MAP_SIZE = 3
actionT DQN::MapIndexToAction(int actionIndex) {
    actionT action;

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
        }
        else{
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
    return action;
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
