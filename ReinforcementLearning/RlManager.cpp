#include "RlManager.h"

void RlManager::SaveModel(){
    //torch::save(target_net, "player.pt"); 
   // torch::save(policy_net, "enemy.pt");
}

void RlManager::LoadModel(){
    //torch::jit::load(target_net, "player.pt");
}

void RlManager::StartPolicy(Map map, Player player, Player enemy) {
  if (calledMemOnce == false) {
    InitializeDQN(map, player, enemy);
    calledMemOnce = true;
  }

  State currentState = CreateCurrentState(map, player, enemy);

  actionT playerAction = policy_net.SelectAction(currentState);
  player.TakeAction(playerAction);
  actionT enemyAction = policy_net.SelectAction(currentState);
  enemy.TakeAction(enemyAction);
  
  State nextState = CreateCurrentState(map, player, enemy);
  State next = State(nextState);

  Transition egal = CreateTransition(currentState, playerAction, next);
  //target_net.PrintWeight();
  memory.push_back(egal);
}

void RlManager::InitializeDQN(Map map, Player player, Player enemy){
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

State RlManager::CreateCurrentState(Map map, Player player, Player enemy) {
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

Transition RlManager::CreateTransition(State s, actionT a, State nextS) {
  Transition t(s, a, nextS);
  return t;
}

std::vector<Transition> RlManager::Sample(size_t batch_size) {
    std::vector<Transition> batch;
    std::sample(memory.begin(), memory.end(), std::back_inserter(batch), batch_size, std::mt19937{std::random_device{}()});
    return batch;
}

void RlManager::OptimizeModel(std::deque<Transition> memory) {
  if (memory.size() < batchSize) {
      return;
  }
  std::vector<Transition> samples;
  std::sample(memory.begin(), memory.end(), std::back_inserter(samples), batchSize, std::mt19937{std::random_device{}()});

  std::vector<torch::Tensor> state_batch, action_batch, reward_batch, next_state_batch;
  for (const auto& t : samples) {
    TensorStruct s(t.state.state);
    TensorStruct ns(t.nextState.state);
    state_batch.push_back(s.GetTensor());
    //action_batch.push_back(torch::tensor(t.stateAction.action, torch::kFloat32)); // fix this shit
    reward_batch.push_back(torch::tensor(t.nextState.reward, torch::kFloat32));
    next_state_batch.push_back(ns.GetTensor());
  }

  torch::Tensor state_tensor = torch::stack(state_batch);
  torch::Tensor action_tensor = torch::stack(action_batch);
  torch::Tensor reward_tensor = torch::stack(reward_batch);
  torch::Tensor next_state_tensor = torch::stack(next_state_batch);

  auto state_action_values = policy_net.Forward(state_tensor).gather(1, action_tensor);
  // torch::Tensor next_state_values = torch::zeros({batchSize}, torch::kFloat32);
  // {
  //     torch::NoGradGuard no_grad; // Disable gradient computation
  //     torch::Tensor non_final_mask = torch::ones({batchSize}, torch::kBool);
  //     for (int i = 0; i < batchSize; ++i) {
  //         if (!next_state_tensor[i].defined()) {
  //             non_final_mask[i] = false;
  //         }
  //     }
  //     auto next_state_values_tensor = target_net.Forward(next_state_tensor);
  //     next_state_values.index_put_({non_final_mask}, next_state_values_tensor.max(1).values.index({non_final_mask}));
  //     next_state_values = next_state_values.detach();
  // }
  // torch::Tensor expected_state_action_values = (next_state_values * gamma) + reward_tensor;
  // torch::nn::SmoothL1Loss criterion;
  // torch::Tensor loss = criterion->forward(state_action_values, expected_state_action_values.unsqueeze(1));
  // optimizer.zero_grad();
  // loss.backward();
  // optimizer.step();
}


