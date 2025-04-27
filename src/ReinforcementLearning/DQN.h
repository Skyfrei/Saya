#ifndef DQN_H
#define DQN_H
#include <deque>
#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>

#include "../Race/Unit/Unit.h"
#include "../State/Player.h"
#include "../Tools/Vec2.h"
#include "Transition.h"

#define MAX_UNITS 50 // 40
#define MAX_STRUCTS 30
#define PEASANT_INDEX_IN_UNITS 10
#define HALL_INDEX_IN_STRCTS 3
#define BARRACK_INDEX_IN_STRUCTS 5
#define MAX_UNITS_TYPE 3

class DQN : public torch::nn::Module
{
  public:
    DQN();
    torch::Tensor Forward(torch::Tensor x);
    void Initialize(Player& pl, Player& en, Map& map);
    actionT SelectAction(Player& pl, Player& en, Map& map); // gotta return an
    void PrintWeight();
    void AttachAgent(Player &pl);
    void Train(Player& pl, Player& en, Map& map);
    void Test();

    void AddExperience(Transition trans);
    void SaveMemory();
    void LoadMemory();
    void SaveMemoryAsBinary();
    void LoadMemoryAsBinary();
    std::deque<Transition> memory;

  private:
    void OptimizeModel(std::deque<Transition> memory);
    State GetState(Player& pl, Player& en, Map& map);
    actionT MapIndexToAction(Player& pl, Player& en, int actionIndex);
    void SaveModel();
    void LoadModel();

  private:
    int episodeNumber = 50;
    int stepsDone = 0;
    int epochNumber = 100;
    float epsilon = 0.9f;
    float epsilonDecay = 1e-3;
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
    TensorStruct(State& state, Map& map);
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
