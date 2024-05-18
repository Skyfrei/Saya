#include "DQN.h"

struct TensorStruct{
  TensorStruct(State state){
    currentMap = GetMapTensor(state.currentMap);
    playerGold = torch::tensor(state.playerGold);
    playerFood = GetVec(state.playerFood);
    playerUnits = GetUnitsTensor(state.playerUnits);
    playerStructs = GetStructuresTensor(state.playerStructs);

    enemyGold = torch::tensor(state.enemyGold);
    enemyFood = GetVec(state.enemyFood);
    enemyUnits = GetUnitsTensor(state.enemyUnits);
    enemyStructures = GetStructuresTensor(state.enemyStructures);
  }
  torch::Tensor currentMap;
  torch::Tensor playerGold;
  torch::Tensor playerFood;
  torch::Tensor playerUnits;
  torch::Tensor playerStructs;
  torch::Tensor enemyGold;
  torch::Tensor enemyFood;
  torch::Tensor enemyUnits;
  torch::Tensor enemyStructures;


  torch::Tensor GetMapTensor(const Map &map) {
    std::vector<float> data;

    for (const auto &row : map.terrain) {
        for (const auto &terrain : row) {
          data.push_back(static_cast<float>(terrain.type)); 
          data.push_back(static_cast<float>(terrain.resourceLeft));
          data.push_back(terrain.coord.x);
          data.push_back(terrain.coord.y);
          for (const auto &unit1 : terrain.onTerrainLiving) {
            Unit* unit = dynamic_cast<Unit*>(unit1);
            data.push_back(unit->health);
            data.push_back(unit->coordinate.x);
            data.push_back(unit->coordinate.y);
            data.push_back(unit->is);
            data.push_back(unit->attack);
            data.push_back(unit->maxHealth);
            data.push_back(unit->mana);
            data.push_back(unit->maxMana);
          }
        if (terrain.structureOnTerrain != nullptr) {
          data.push_back(terrain.structureOnTerrain->health);
          data.push_back(terrain.structureOnTerrain->coordinate.x);
          data.push_back(terrain.structureOnTerrain->coordinate.y);
          data.push_back(terrain.structureOnTerrain->is);
        } 
        else {
          data.push_back(0.0f);  
          data.push_back(0.0f); 
          data.push_back(0.0f);  
          data.push_back(0.0f); 
        }
      }
    }
    return torch::tensor(data).view({-1, static_cast<int>(data.size() / map.terrain.size() / map.terrain[0].size())});
  }

  torch::Tensor GetVec(const Vec2 food) {
    std::vector<float> data;
    data.push_back(food.x);
    data.push_back(food.y);
    return torch::tensor(data).view({-1, 2});
  }

  torch::Tensor GetUnitsTensor(const std::vector<Unit *> &units) {
    std::vector<float> data;
    for (const auto &unit : units) {
        // Extract health and coordinates from each structure
        data.push_back(unit->health);
        data.push_back(unit->coordinate.x);
        data.push_back(unit->coordinate.y);
        data.push_back(unit->is);
        data.push_back(unit->attack);
        data.push_back(unit->maxHealth);
        data.push_back(unit->mana);
        data.push_back(unit->maxMana);
    }
    return torch::tensor(data).view({-1, 8});
  }

  torch::Tensor GetStructuresTensor(const std::vector<Structure *> &structures) {
    std::vector<float> structureData;
    for (const auto &structure : structures) {
        structureData.push_back(structure->health);
        structureData.push_back(structure->coordinate.x);
        structureData.push_back(structure->coordinate.y);
        structureData.push_back(structure->is);
    }
    return torch::tensor(structureData).view({-1, 4});
  }

  torch::Tensor GetTensor(){
    return torch::cat({currentMap, playerGold, playerFood, playerUnits, playerStructs, enemyGold, enemyFood, enemyUnits, enemyStructures});
  }
};

DQN::DQN(int inpSize, int actionNum) {
  inputSize = inpSize;
  actionSize = actionNum;

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

at::Tensor DQN::SelectAction(State state, DQN policy, torch::Device device) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist1(0.0, 1.0);

  float epsilonThreshold = endEpsilon + (startEpsilon - endEpsilon) * std::exp(-1. * stepsDone / epsilonDecay);
  stepsDone++;  
  float sample = dist1(rng);

  if (sample > epsilonThreshold) {
    at::Tensor action = std::get<1>(policy.Forward(TurnStateInInput(state)).max(1)).view({1, 1});

    return action;
  } 
  else {
    at::Tensor action = torch::randint(0, actionSize - 1, {1}).to(torch::kLong).to(device);
    return action;
  }
}

torch::Tensor DQN::TurnStateInInput(State state){
  TensorStruct ts(state);
  return ts.GetTensor();
}