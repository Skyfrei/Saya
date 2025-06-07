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
#include "../Tools/Macro.h"
#include "../Tools/Enums.h"
#include "Tensor.h"


class DQN : public torch::nn::Module
{
  public:
    DQN();
    torch::Tensor Forward(torch::Tensor x);
    void Initialize(Map &map, State &s);
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

    int mapSize = MAP_SIZE * MAP_SIZE;
    int moveAction = MAP_SIZE * MAP_SIZE * MAX_UNITS;
    int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
    int buildAction =
        attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE;
    int farmAction =
        buildAction + PEASANT_INDEX_IN_UNITS * mapSize *
                          HALL_INDEX_IN_STRCTS; // town hall size multipled here as well
    int recruitAction =
        farmAction + 2 * NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size
};

#endif
