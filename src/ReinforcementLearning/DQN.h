#ifndef DQN_H
#define DQN_H
#include <deque>
#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>
#include <tuple>


#include "../Race/Unit/Unit.h"
#include "../State/Player.h"
#include "../Tools/Vec2.h"
#include "Transition.h"

#define MAX_UNITS 50 // 40
#define MAX_STRUCTS 30
#define PEASANT_INDEX_IN_UNITS 10
#define HALL_INDEX_IN_STRCTS 5
#define BARRACK_INDEX_IN_STRUCTS 5
#define MAX_UNITS_TYPE 3

class DQN : public torch::nn::Module
{
  public:
    DQN();
    torch::Tensor Forward(torch::Tensor x);
    void Initialize(Player &pl, Player &en, Map &map, State &s);
    std::tuple<actionT, int> SelectAction(Player &pl, Player &en, Map &map, State &s,
                         float epsilon); // gotta return an
    actionT SelectAction(State &state, Map &map, float epsilon);
    actionT MapIndexToAction(State &state, int actionIndex);
    int GetRandomOutputIndex();

    void PrintWeight();
    void AttachAgent(Player &pl);
    void SaveModel();
    void LoadModel();


  private:
    actionT MapIndexToAction(Player &pl, Player &en, int actionIndex);
    
  public:
    int episodeNumber = 50;
    int stepsDone = 0;
    int actionSize = 0;
    int inputSize = 0;
    torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};

    int memory_size = 1000;
    const std::string memory_file = "dqn_memory.say";
    const std::string memory_file_binary = "binary.bay";
    const std::string model_file = "model.pt";
};

struct TensorStruct
{
    TensorStruct(State &state, Map &map);
    torch::Tensor GetMapTensor(Map &map);
    torch::Tensor GetVec(Vec2 food);
    torch::Tensor GetUnitsTensor(std::vector<Unit *> &units);
    torch::Tensor GetStructuresTensor(std::vector<Structure *> &structures);
    torch::Tensor GetTensor();
    const int unitVar = 8;
    const int strucVar = 4;

    torch::Tensor currentMap;
    torch::Tensor playerGold;
    torch::Tensor playerFood;
    torch::Tensor playerUnits;
    torch::Tensor playerStructs;
    torch::Tensor enemyGold;
    torch::Tensor enemyFood;
    torch::Tensor enemyUnits;
    torch::Tensor enemyStructs;
};

#endif
