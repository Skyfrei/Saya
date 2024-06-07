#include "ReplayMemory.h"

void ReplayMemory::InitializeDQN(Map map, Player &player, Player &enemy){
  State st = CreateCurrentState(map, player, enemy);

  policy_net.Initialize(st);
  target_net.Initialize(st);

  torch::Device device(torch::kCPU);
  if (torch::cuda::is_available()) {
    device = torch::Device(torch::DeviceType::CUDA);
  }
  policy_net.to(device);
  target_net.to(device);
  torch::optim::AdamW optimizer(policy_net.parameters(), torch::optim::AdamWOptions(0.01).weight_decay(1e-4));
}

void ReplayMemory::StartPolicy(Map map, Player &player, Player &enemy) {
  if (!calledMemOnce) {
    InitializeDQN(map, player, enemy);
    calledMemOnce = true;
  }
  State currentState = CreateCurrentState(map, player, enemy);
  actionT playerAction = policy_net.SelectAction(currentState);
  player.TakeAction(playerAction);
  actionT enemyAction = policy_net.SelectAction(currentState);
  enemy.TakeAction(enemyAction);

  State nextState = CreateCurrentState(map, player, enemy);
  NextState next = NextState(nextState, 0.0f);

  Transition egal = CreateTransition(currentState, playerAction, next);

  memory.push_back(egal);
}

Transition ReplayMemory::CreateTransition(State s, actionT a, NextState nextS) {
  Transition t(s, a, nextS);
  return t;
}

std::vector<Transition> ReplayMemory::Sample(size_t batch_size) {
    std::vector<Transition> batch;
    std::sample(memory.begin(), memory.end(), std::back_inserter(batch), batch_size, std::mt19937{std::random_device{}()});
    return batch;
}



State ReplayMemory::CreateCurrentState(Map map, Player &player, Player &enemy) {
  State st;
  st.currentMap = map;
  st.playerFood = player.food;
  st.playerGold = player.gold;

  st.enemyGold = enemy.gold;
  st.enemyFood = enemy.food;
  for (int i = 0; i < player.units.size(); i++) {
    Footman *footman = nullptr;
    Peasant *peasant = nullptr;
    switch (player.units[i]->is) {
      case FOOTMAN:
        footman = dynamic_cast<Footman *>(player.units[i].get());
        st.playerUnits.push_back(new Footman(*footman));
        break;
      case PEASANT:
        peasant = dynamic_cast<Peasant *>(player.units[i].get());
        st.playerUnits.push_back(new Peasant(*peasant));
        break;
      default:
        break;
    }
  }
  for (int i = 0; i < enemy.units.size(); i++) {
    Footman *footman = nullptr;
    Peasant *peasant = nullptr;
    switch (enemy.units[i]->is) {
      case FOOTMAN:
        footman = dynamic_cast<Footman *>(enemy.units[i].get());
        st.enemyUnits.push_back(new Footman(*footman));
        break;
      case PEASANT:
        peasant = dynamic_cast<Peasant *>(enemy.units[i].get());
        st.enemyUnits.push_back(new Peasant(*peasant));
        break;
      default:
        break;
    }
  }

  for (int i = 0; i < player.structures.size(); i++) {
    Barrack *barracks = nullptr;
    Farm *farm = nullptr;
    TownHall *townHall = nullptr;
    switch (player.structures[i]->is) {
      case BARRACK:
        barracks = dynamic_cast<Barrack *>(player.structures[i].get());
        st.playerStructs.push_back(new Barrack(*barracks));
        break;
      case FARM:
        farm = dynamic_cast<Farm *>(player.structures[i].get());
        st.playerStructs.push_back(new Farm(*farm));
        break;
      case HALL:
        townHall = dynamic_cast<TownHall *>(player.structures[i].get());
        st.playerStructs.push_back(new TownHall(*townHall));
        break;
      default:
        break;
    }
  }

  for (int i = 0; i < enemy.structures.size(); i++) {
    Barrack *barracks = nullptr;
    Farm *farm = nullptr;
    TownHall *townHall = nullptr;
    switch (enemy.structures[i]->is) {
      case BARRACK:
        barracks = dynamic_cast<Barrack *>(enemy.structures[i].get());
        st.enemyStructures.push_back(new Barrack(*barracks));
        break;
      case FARM:
        farm = dynamic_cast<Farm *>(enemy.structures[i].get());
        st.enemyStructures.push_back(new Farm(*farm));
        break;
      case HALL:
        townHall = dynamic_cast<TownHall *>(enemy.structures[i].get());
        st.enemyStructures.push_back(new TownHall(*townHall));
        break;
      default:
        break;
    }
  }
  return st;
}
