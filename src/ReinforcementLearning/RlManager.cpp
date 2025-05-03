#include "RlManager.h"

#include <chrono>
#include <fstream>
#include <random>

#include "Reward.h"

RlManager::RlManager() {
}

void RlManager::InitializeDQN(Player &pl, Player &en, Map &map) {
    State s = GetState(pl, en, map);
    policyNet.Initialize(pl, en, map, s);
    targetNet.Initialize(pl, en, map, s);
    torch::Device device(torch::kCPU);

    if (torch::cuda::is_available())
    {
        device = torch::Device(torch::DeviceType::CUDA);
        episodeNumber = 200;
    }

    policyNet.to(device);
    targetNet.to(device);
}

void RlManager::TrainDQN(Player &pl, Player &en, Map &map) {
    float loss = 0.0f;
    float discount = 0.7f;
    int batch_size = 100;
    torch::optim::AdamW optimizer(policyNet.parameters(),
                                  torch::optim::AdamWOptions(0.01).weight_decay(1e-4));
    std::random_device dev;
    std::mt19937 rng(dev());

    for (int i = 0; i < episodeNumber; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            std::deque<Transition> samples;
            std::sample(memory.begin(), memory.end(), std::back_inserter(samples),
                        batch_size, std::mt19937{std::random_device{}()});
            std::uniform_int_distribution<std::mt19937::result_type> sampleIndex(
                0, batch_size - 1);

            std::vector<torch::Tensor> state_batch;
            std::vector<torch::Tensor> state_action;
            std::vector<torch::Tensor> next_state_batch;
            std::vector<torch::Tensor> reward_batch;

            for (auto &trans : samples)
            {
                TensorStruct z(trans.state, map);
                state_batch.push_back(z.GetTensor());
                state_action.push_back(torch::tensor({trans.actionIndex}, torch::kInt32));
                reward_batch.push_back(torch::tensor({trans.reward}, torch::kFloat32));
            }
            torch::Tensor tensor_states = torch::stack(state_batch);
            torch::Tensor tensor_actions = torch::stack(state_action);
            torch::Tensor tensor_rewards = torch::stack(reward_batch);

            torch::Tensor q_values = policyNet.Forward(tensor_states);
            torch::Tensor actions_reshaped = tensor_actions.view({batch_size, 1});
            torch::Tensor action_values = q_values.gather(1, actions_reshaped);

            for (auto &trans : samples)
            {
                TensorStruct z(trans.nextState, map);
                next_state_batch.push_back(z.GetTensor());
            }
            torch::Tensor tensor_next_states = torch::stack(next_state_batch);
            torch::NoGradGuard no_grad;
            torch::Tensor q_next_values =
                std::get<0>(targetNet.Forward(tensor_next_states).max(1));
            q_next_values = (q_next_values * gamma) + tensor_rewards;

            auto criterion = torch::nn::SmoothL1Loss();
            auto loss = criterion(action_values, q_next_values.unsqueeze(1));
            std::cout << "Loss: " << loss.item<float>() << std::endl;
            optimizer.zero_grad();
            loss.backward();
            torch::nn::utils::clip_grad_value_(policyNet.parameters(), 100);
            optimizer.step();

            // int sample_index = sampleIndex(rng);
            // Transition &trans = memory[sample_index];
            // float reward = GetRewardFromAction(trans.action);
            // actionT next_action =
            //     targetNet.SelectAction(pl, en, map, trans.nextState, epsilon);
            // float next_reward = (GetRewardFromAction(next_action) * gamma) +
            // trans.reward;

            // torch::Tensor reward_tensor =
            //     torch::tensor({reward}, torch::TensorOptions().requires_grad(true));
            // torch::Tensor next_reward_tensor =
            //     torch::tensor({next_reward},
            //     torch::TensorOptions().requires_grad(true));

            if (epsilon - epsilonDecay > 0)
                epsilon -= epsilonDecay;
        }
    }
}

State RlManager::GetState(Player &pl, Player &en, Map &map) {
    State state;
    state.playerGold = pl.gold;
    state.playerFood.x = pl.food.x;
    state.playerFood.y = pl.food.y;
    state.enemyGold = en.gold;
    state.enemyFood.x = en.food.x;
    state.enemyFood.y = en.food.y;
    state.playerUnits.resize(pl.units.size());
    state.enemyUnits.resize(en.units.size());
    state.playerStructs.resize(pl.structures.size());
    state.enemyStructs.resize(en.structures.size());

    for (int i = 0; i < pl.units.size(); i++)
        state.playerUnits[i] = pl.units[i].get();

    for (int i = 0; i < pl.structures.size(); i++)
        state.playerStructs[i] = pl.structures[i].get();

    for (int i = 0; i < en.units.size(); i++)
        state.enemyUnits[i] = en.units[i].get();

    for (int i = 0; i < en.structures.size(); i++)
        state.enemyStructs[i] = en.structures[i].get();

    return state;
}

void RlManager::AddExperience(Transition trans) {
    if (memory.size() >= memory_size)
    {
        memory.pop_front();
    }
    memory.push_back(trans);
}

void RlManager::SaveMemory() {
    std::string data_to_save = "";
    for (int i = 0; i < memory.size(); i++)
    {
        data_to_save += memory[i].Serialize() + "\n";
    }
    std::ofstream file;
    file.open(memory_file);
    file << data_to_save;
    file.close();
}

void RlManager::LoadMemory() {
    std::ifstream file(memory_file);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open())
    {
        std::cout << "String replay file couldn't be opened.";
        return;
    }
    while (std::getline(file, line))
    {
        Transition trans;
        trans = trans.Deserialize(line);
        // trans = trans.Deserialize(line);
        // AddExperience(trans);
    }
    file.close();
}

void RlManager::SaveMemoryAsBinary() {
    std::vector<binary> data_to_save;
    std::ofstream file;
    file.open(memory_file_binary, std::ios::binary);

    for (int i = 0; i < memory.size(); i++)
    {
        std::vector<binary> data = memory[i].SerializeBinary();
        file.write(reinterpret_cast<char *>(data.data()), data.size() * sizeof(binary));
    }

    file.close();
}
// 0-11 first bytes,
void RlManager::LoadMemoryAsBinary() {
    std::ifstream file(memory_file_binary, std::ios::binary);
    std::vector<binary> binaryData;
    int expectedBytes = 0;
    binary temp;
    int count = 0;

    if (!file.is_open())
    {
        std::cout << "Binary replay file couldn't be opened.";
        return;
    }

    while (file.read(reinterpret_cast<char *>(&temp), sizeof(binary)))
    {
        if (binaryData.size() == 0)
        {
            expectedBytes = std::get<int>(temp);
            binaryData.resize(expectedBytes);
            // std::cout<<expectedBytes<<" ";
            continue;
        }
        binaryData[count] = temp;

        if (count == expectedBytes - 1)
        {
            Transition trans;
            trans = trans.DeserializeBinary(binaryData);
            AddExperience(trans);
            binaryData.clear();
            count = 0;
            continue;
        }
        count++;
    }
    file.close();
}

// double RlManager::CalculateStateReward(State state) {
//     double reward = 0.0;
//
//     if (state.enemyUnits.size() <= 0 && state.enemyStructs.size() <= 0)
//         reward = 1;
//     else if (state.playerUnits.size() <= 0 && state.playerStructs.size() <=
//     0)
//         reward = -1;
//
//     return reward;
// }

// void RlManager::InitializePPO() {
// }
//
// State RlManager::CreateCurrentState(Map &map, Player &player, Player &enemy)
// {
//     State st;
//     // st.currentMap = map;
//     st.playerFood = player.food;
//     st.playerGold = player.gold;
//
//     st.enemyGold = enemy.gold;
//     st.enemyFood = enemy.food;
//     for (int i = 0; i < player.units.size(); i++)
//     {
//         Footman *footman = nullptr;
//         Peasant *peasant = nullptr;
//         switch (player.units[i]->is)
//         {
//         case FOOTMAN:
//             footman = dynamic_cast<Footman *>(player.units[i].get());
//             st.playerUnits.push_back(new Footman(*footman));
//             break;
//         case PEASANT:
//             peasant = dynamic_cast<Peasant *>(player.units[i].get());
//             st.playerUnits.push_back(new Peasant(*peasant));
//             break;
//         default:
//             break;
//         }
//     }
//     for (int i = 0; i < enemy.units.size(); i++)
//     {
//         Footman *footman = nullptr;
//         Peasant *peasant = nullptr;
//         switch (enemy.units[i]->is)
//         {
//         case FOOTMAN:
//             footman = dynamic_cast<Footman *>(enemy.units[i].get());
//             st.enemyUnits.push_back(new Footman(*footman));
//             break;
//         case PEASANT:
//             peasant = dynamic_cast<Peasant *>(enemy.units[i].get());
//             st.enemyUnits.push_back(new Peasant(*peasant));
//             break;
//         default:
//             break;
//         }
//     }
//
//     for (int i = 0; i < player.structures.size(); i++)
//     {
//         Barrack *barracks = nullptr;
//         Farm *farm = nullptr;
//         TownHall *townHall = nullptr;
//         switch (player.structures[i]->is)
//         {
//         case BARRACK:
//             barracks = dynamic_cast<Barrack *>(player.structures[i].get());
//             st.playerStructs.push_back(new Barrack(*barracks));
//             break;
//         case FARM:
//             farm = dynamic_cast<Farm *>(player.structures[i].get());
//             st.playerStructs.push_back(new Farm(*farm));
//             break;
//         case HALL:
//             townHall = dynamic_cast<TownHall *>(player.structures[i].get());
//             st.playerStructs.push_back(new TownHall(*townHall));
//             break;
//         default:
//             break;
//         }
//     }
//
//     for (int i = 0; i < enemy.structures.size(); i++)
//     {
//         Barrack *barracks = nullptr;
//         Farm *farm = nullptr;
//         TownHall *townHall = nullptr;
//         switch (enemy.structures[i]->is)
//         {
//         case BARRACK:
//             barracks = dynamic_cast<Barrack *>(enemy.structures[i].get());
//             st.enemyStructs.push_back(new Barrack(*barracks));
//             break;
//         case FARM:
//             farm = dynamic_cast<Farm *>(enemy.structures[i].get());
//             st.enemyStructs.push_back(new Farm(*farm));
//             break;
//         case HALL:
//             townHall = dynamic_cast<TownHall *>(enemy.structures[i].get());
//             st.enemyStructs.push_back(new TownHall(*townHall));
//             break;
//         default:
//             break;
//         }
//     }
//     return st;
// }
//
// Transition RlManager::CreateTransition(State s, actionT a, State nextS) {
//     Transition t(s, a, nextS);
//     return t;
// }

// void RlManager::OptimizeModel(std::deque<Transition> memory) {
//   if (memory.size() < batchSize) {
//       return;
//   }
//   std::vector<Transition> samples = Sample(batchSize);
//
//   std::vector<torch::Tensor> state_batch, action_batch, reward_batch,
//   next_state_batch; for (const auto& t : samples) {
//     TensorStruct s(t.state);
//     TensorStruct ns(t.nextState);
//     state_batch.push_back(s.GetTensor());
//     //action_batch.push_back(torch::tensor(t.stateAction.action,
//     torch::kFloat32)); // fix this shit
//     reward_batch.push_back(torch::tensor(t.nextState.reward,
//     torch::kFloat32)); next_state_batch.push_back(ns.GetTensor());
//   }
//
//   torch::Tensor state_tensor = torch::stack(state_batch);
//   torch::Tensor action_tensor = torch::stack(action_batch);
//   torch::Tensor reward_tensor = torch::stack(reward_batch);
//   torch::Tensor next_state_tensor = torch::stack(next_state_batch);
//
//   auto state_action_values = policy_net.Forward(state_tensor).gather(1,
//   action_tensor);
//   // torch::Tensor next_state_values = torch::zeros({batchSize},
//   torch::kFloat32);
//   // {
//   //     torch::NoGradGuard no_grad; // Disable gradient computation
//   //     torch::Tensor non_final_mask = torch::ones({batchSize},
//   torch::kBool);
//   //     for (int i = 0; i < batchSize; ++i) {
//   //         if (!next_state_tensor[i].defined()) {
//   //             non_final_mask[i] = false;
//   //         }
//   //     }
//   //     auto next_state_values_tensor =
//   target_net.Forward(next_state_tensor);
//   //     next_state_values.index_put_({non_final_mask},
//   next_state_values_tensor.max(1).values.index({non_final_mask}));
//   //     next_state_values = next_state_values.detach();
//   // }
//   // torch::Tensor expected_state_action_values = (next_state_values * gamma)
//   + reward_tensor;
//   // torch::nn::SmoothL1Loss criterion;
//   // torch::Tensor loss = criterion->forward(state_action_values,
//   expected_state_action_values.unsqueeze(1));
//   // optimizer.zero_grad();
//   // loss.backward();
//   // optimizer.step();
// }
