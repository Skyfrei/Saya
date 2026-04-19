#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include "../State/Map.h"
#include "../State/Player.h"
#include "DQN.h"
#include "PPO.h"
#include "../gui/Window.h"
#include "ValueNetwork.h"
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
    void TrainPPO(Player &pl, Player &en, Map &map);

    void AddExperience(Transition trans);
    void SaveMemoryAsString();
    void LoadMemoryAsString();
    void SaveMemoryAsBinary();
    void LoadMemoryAsBinary();
    actionT GetActionPPO(Player&, Player&, Map&);
    actionT GetActionDQNEnemy(Player& en, Player& pl, Map& map);
    at::Tensor GetPPOTensor(Player&, Player&, Map&);
    at::Tensor GetDQNTensor(Player&, Player&, Map&);

    void ShowInMap(Player& pl, Player& en, Map& m, at::Tensor& tensor, std::string mode = "ppo");
    void ShowInMap(Player& pl, Player& en, Map& m, at::Tensor& tensor, at::Tensor& tensordqn);
    bool ShouldResetEnvironment(Player &pl, Player &en, Map &map);
    actionT GetActionPPOEnemy(Player& pl, Player& en, Map& map);

    actionT GetActionDQN(Player& pl, Player& en, Map& map);

  public:
    std::deque<Transition> memory;
    DQN policyNet;
    DQN targetNet;
    
    std::deque<Transition> ppoMemory;
    PPO ppoPolicy;
    PPO enemyPPO;
    ValueNetwork ppoValue;

  private:
    State CreateCurrentState(Map &map, Player &player, Player &enemy);
    Transition CreateTransition(State s, actionT a, State rextS);
    State GetState(Player &pl, Player &en, Map &map);
    void OptimizeDQN(Map &map, torch::optim::AdamW&);
    at::Tensor GetMask(Player&, Player&, int);

  private:
    int frameSkipCounter = 0;
    const int frameSkipLimit = 4; // AI only "thinks" every 4 frames
    actionT lastAction;
    at::Tensor cachedMask;

  private:
    const int batchSize = 32; // was 32
    float gamma = 0.98f;
    float epsilon = 0.9f;
    bool calledMemOnce = false;
    const int maxSize = 10000;
    int memory_size = maxSize;

    float ppoEpsilon = 0.2f;
    float epsilonDecay = 0;
    int episodeNumber = 50;
    int forwardSteps = 256;
    Window win;
};
#endif
