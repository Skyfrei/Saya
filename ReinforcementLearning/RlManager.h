#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include <torch/cuda.h>
#include <torch/optim/adamw.h>
#include <torch/torch.h>

#include <deque>
#include <chrono>
#include <memory>
#include <variant>
#include <fstream>

#include "../State/Map.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Peasant.h"
#include "../State/Player.h"
#include "../Race/Structure/TownHall.h"
#include "DQN.h"
#include "Transition.h"



class RlManager {
  public:
    RlManager(){SaveModel();}
    void InitializeDQN(Map map, Player player, Player enemy);
    void StartPolicy(Map m, Player player, Player enemy);
    std::vector<Transition> Sample(size_t batch_size);
    void AddMemory(const Transition& experience);
    size_t Size() const { return memory.size(); }
    void OptimizeModel(std::deque<Transition> memory);
    void SaveModel();
    void LoadModel();
    void Serialize(const State& state);
    void Deserialize(const State& state);
    void SaveExperienceBuffer();
    void LoadExperienceBuffer();
    double CalculateStateReward(State state);

  private:
    State CreateCurrentState(Map map, Player player, Player enemy);
    Transition CreateTransition(State s, actionT a, State nextS);
    void SerializeVec2(std::ostream&, const Vec2&);
    void DeserializeVec2(std::istream&, Vec2&);
    void SerializeTerrain(std::ostream&, const Terrain&);
    void DeserializeTerrain(std::istream&, Terrain&);
    void SerializeMap(std::ostream&, const Map&);
    void DeserializeMap(std::istream&, Map&);
    void SerializeStructure(std::ostream&, const Structure&);
    void DeserializeStructure(std::istream&, Structure&);
    void SerializeTransition(std::ostream&, const Transition&);
    void DeserializeTransition(std::istream&, Transition&);
    void SerializeState(std::ostream&, const State&);
    void DeserializeState(std::istream&, State&);
    void SerializeUnit(std::ostream&, const Unit*);
    void DeserializeUnit(std::istream&, const Unit*);

  private: 
    std::deque<Transition> memory;
    DQN policy_net;
    DQN target_net;

    float gamma = 0.92f;
    bool calledMemOnce = false;
    const int batchSize = 32;
    const int maxSize = 10000;
    const std::string fileName = "memory.bin";
};
#endif
