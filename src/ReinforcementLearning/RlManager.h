#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include "../State/Map.h"
#include "../State/Player.h"
#include "DQN.h"
#include "Transition.h"
#include <deque>

class RlManager
{
  public:
    RlManager();
    void InitializeDQN(Player &pl, Player &en, Map &map);
    void InitializePPO(Player &pl, Player &en, Map &map);
    double CalculateStateReward(State state);
    void TrainDQN(Player &pl, Player &en, Map &map);

    void AddExperience(Transition trans);
    void SaveMemory();
    void LoadMemory();
    void SaveMemoryAsBinary();
    void LoadMemoryAsBinary();

  public:
    std::deque<Transition> memory;
    DQN policyNet;
    DQN targetNet;

  private:
    State CreateCurrentState(Map &map, Player &player, Player &enemy);
    Transition CreateTransition(State s, actionT a, State rextS);
    State GetState(Player &pl, Player &en, Map &map);

  private:
    float gamma = 0.92f;
    bool calledMemOnce = false;
    const int batchSize = 32;
    const int maxSize = 10000;
    int memory_size = 100000;
    const std::string memory_file = "dqn_memory.say";
    const std::string memory_file_binary = "binary.bay";
    float epsilon = 0.9f;
    float epsilonDecay = 1e-3;
    int episodeNumber = 10;
};
#endif
