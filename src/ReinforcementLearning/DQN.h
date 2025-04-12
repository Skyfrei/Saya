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
    void Initialize(State state);
    actionT SelectAction(State state); // gotta return an
    void PrintWeight();
    void AttachAgent(Player &pl);
    void Train();
    void Test();

    void AddExperience(Transition trans);
    void SaveMemory();
    void LoadMemory();
    void SaveMemoryAsBinary();
    void LoadMemoryAsBinary();
    std::deque<Transition> memory;

  private:
    void OptimizeModel(std::deque<Transition> memory);
    torch::Tensor TurnStateInInput(State state);
    actionT MapIndexToAction(int actionIndex);

  private:
    int episodeNumber = 50;
    int stepsDone = 0;
    int epochNumber = 100;
    float startEpsilon = 0.9f;
    float endEpsilon = 0.05f;
    float epsilonDecay = 1000.0f;
    float learningRate = 0.01f;
    int actionSize = 0;
    int inputSize = 0;
    torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};

    int memory_size = 1000;
    const std::string memory_file = "dqn_memory.say";
    const std::string memory_file_binary = "binary.bay";
};

struct TensorStruct
{
    TensorStruct(const State &state) {
        //currentMap = GetMapTensor(state.currentMap);
        playerGold = torch::tensor(state.playerGold).view({-1, 1});
        playerFood = GetVec(state.playerFood);
        playerUnits = GetUnitsTensor(state.playerUnits);
        playerStructs = GetStructuresTensor(state.playerStructs);

        enemyGold = torch::tensor(state.enemyGold).view({-1, 1});
        enemyFood = GetVec(state.enemyFood);
        enemyUnits = GetUnitsTensor(state.enemyUnits);
        enemyStructs = GetStructuresTensor(state.enemyStructs);
    }

    torch::Tensor GetMapTensor(const Map &map) {
        std::vector<int> data;

        for (const auto &row : map.terrain)
        {
            for (const auto &terrain : row)
            {
                data.push_back((terrain.type));
                data.push_back((terrain.resourceLeft));
                data.push_back(terrain.coord.x);
                data.push_back(terrain.coord.y);
            }
        }
        return torch::tensor(data).view({1, -1});
    }

    torch::Tensor GetVec(const Vec2 food) {
        std::vector<int> data;
        data.push_back(food.x);
        data.push_back(food.y);
        return torch::tensor(data).view({1, -1});
    }

    torch::Tensor GetUnitsTensor(const std::vector<Unit *> &units) {
        std::vector<int> data;
        for (const auto &unit : units)
        {
            // Extract health and coordinates from each structure
            data.push_back(static_cast<int>(unit->health));
            data.push_back(unit->coordinate.x);
            data.push_back(unit->coordinate.y);
            data.push_back(unit->is);
            data.push_back(static_cast<int>(unit->attack));
            data.push_back(static_cast<int>(unit->maxHealth));
            data.push_back(static_cast<int>(unit->mana));
            data.push_back(static_cast<int>(unit->maxMana));
        }
        return torch::tensor(data).view({1, -1});
    }

    torch::Tensor GetStructuresTensor(const std::vector<Structure *> &structures) {
        std::vector<int> structureData;
        for (const auto &structure : structures)
        {
            structureData.push_back(static_cast<int>(structure->health));
            structureData.push_back(structure->coordinate.x);
            structureData.push_back(structure->coordinate.y);
            structureData.push_back(structure->is);
        }
        return torch::tensor(structureData).view({1, -1});
    }

    torch::Tensor GetTensor() {
        torch::Tensor paddedUnits = torch::zeros({1, (MAX_UNITS - playerUnits.size(1) / unitVar) * unitVar});
        torch::Tensor paddedStructs = torch::zeros({1, (MAX_STRUCTS - playerStructs.size(1) / strucVar) * strucVar});
        torch::Tensor paddedUnitsEnemy = torch::zeros({1, (MAX_UNITS - enemyUnits.size(1) / unitVar) * unitVar});
        torch::Tensor paddedStructsEnemy =
            torch::zeros({1, (MAX_STRUCTS - enemyStructs.size(1) / strucVar) * strucVar});

        std::vector<torch::Tensor> tensors = {
            playerGold, playerFood, playerUnits,      paddedUnits,  playerStructs,     paddedStructs,
            enemyGold,  enemyFood,  enemyUnits, paddedUnitsEnemy, enemyStructs, paddedStructsEnemy};

        torch::Tensor concatenatedTensor = torch::cat(tensors, 1);

        return concatenatedTensor;
    }

    const int unitVar = 8;
    const int strucVar = 4;

    //torch::Tensor currentMap;
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
