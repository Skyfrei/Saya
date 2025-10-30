#include "RlManager.h"

#include "Reward.h"
#include <chrono>
#include <fstream>
#include <random>
#include <tuple>
#include "Tensor.h"
#include <algorithm>

RlManager::RlManager() : win(Vec2(1000, 1000)) {
}

void RlManager::InitializeDQN(Player &pl, Player &en, Map &map) {
    State s = GetState(pl, en, map);
    policyNet.Initialize(map, s);
    targetNet.Initialize(map, s);
    torch::Device device(torch::kCPU);

    if (torch::cuda::is_available())
    {
        device = torch::Device(torch::DeviceType::CUDA);
        episodeNumber = 200;
        policyNet.to(device);
        targetNet.to(device);
    }

}

void RlManager::InitializePPO(Player &pl, Player &en, Map &map){
    State s = GetState(pl, en, map);
    ppoPolicy.Initialize(map, s);
    ppoValue.Initialize(ppoPolicy);
    torch::Device device(torch::kCPU);

    if (torch::cuda::is_available())
    {
        device = torch::Device(torch::DeviceType::CUDA);
        episodeNumber = 200;
        ppoPolicy.to(device);
        ppoValue.to(device);
    }

}

void RlManager::OptimizeDQN(Map &map) {
    int batch_size = 512;
    torch::optim::AdamW optimizer(
        policyNet.parameters(), torch::optim::AdamWOptions(0.01).weight_decay(1e-4));
    std::random_device dev;
    std::mt19937 rng(dev());

    std::deque<Transition> samples;
    std::sample(memory.begin(), memory.end(), std::back_inserter(samples),
                batch_size, rng);

    std::vector<torch::Tensor> state_batch;
    std::vector<torch::Tensor> state_action;
    std::vector<torch::Tensor> next_state_batch;
    std::vector<torch::Tensor> reward_batch;

    state_batch.reserve(batch_size);
    next_state_batch.reserve(batch_size);
    for (auto &trans : samples)
    {
        TensorStruct z(trans.state, map);
        TensorStruct z1(trans.nextState, map);
        state_batch.push_back(z.GetTensor());
        state_action.push_back(torch::tensor({trans.actionIndex}));
        reward_batch.push_back(torch::tensor({trans.reward}, torch::kFloat32));
        next_state_batch.push_back(z1.GetTensor());
    }

    torch::Tensor tensor_states = torch::cat(state_batch);
    torch::Tensor tensor_actions = torch::cat(state_action).unsqueeze(1);
    torch::Tensor tensor_rewards = torch::cat(reward_batch).unsqueeze(1);
    torch::Tensor q_values =
        policyNet.Forward(tensor_states).gather(1, tensor_actions);

    torch::Tensor tensor_next_states = torch::cat(next_state_batch);
    torch::Tensor q_next_values;
    {
        torch::NoGradGuard no_grad;
        q_next_values =
            std::get<0>(targetNet.Forward(tensor_next_states).max(1)).unsqueeze(1);
    }
    q_next_values = (q_next_values * gamma) + tensor_rewards;

    auto criterion = torch::nn::SmoothL1Loss();
    auto loss = criterion(q_values, q_next_values);

    std::ofstream file("experiment_loss_dqn.txt", std::ios::app);
    std::cout << "Loss: " << loss.item<float>() << std::endl;
    file << loss.item<float>() << "\n";
    optimizer.zero_grad();
    loss.backward();
    torch::nn::utils::clip_grad_value_(policyNet.parameters(), 100);
    optimizer.step();
}

bool RlManager::ShouldResetEnvironment(Player &pl, Player &en, Map &map, float &reward) {
    if (!(pl.HasUnit(PEASANT) && pl.HasStructure(HALL))){
        pl.units.clear();
        pl.structures.clear();
        en.units.clear();
        en.structures.clear();
        pl.Initialize();
        en.Initialize();
        reward = reward - 10.0f;
        std::cout << "End state reached";

        return true;
    }else if (!(en.HasUnit(PEASANT) && en.HasStructure(HALL))){
        pl.units.clear();
        pl.structures.clear();
        en.units.clear();
        en.structures.clear();
        pl.Initialize();
        en.Initialize();
        reward = reward + 10.0f;
        std::cout << "End state reached";

        return true;
    }
    return false;
}
void RlManager::TrainPPO(Player &pl, Player &en, Map &map){
    torch::optim::AdamW policy_optimizer(
        ppoPolicy.parameters(), torch::optim::AdamWOptions(3e-4).weight_decay(1e-4));
    torch::optim::AdamW value_optimizer(
        ppoValue.parameters(), torch::optim::AdamWOptions(3e-4).weight_decay(1e-4));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, ppoPolicy.layer3->options.out_features() - 1);
    std::ofstream file;
    file.open("ppo_loss.txt", std::ios_base::app);
    bool done = false;

    
    for (int i = 0; i < episodeNumber; i++){
        done = false;
        while (!done){
            State s = GetState(pl, en, map);


            Player playerClone = Player(pl);
            Player enemyClone = Player(en);
            std::vector<std::tuple<State, int, at::Tensor, at::Tensor>> index_old_probabilities;
            
            TensorStruct old_input(s, map);

            at::Tensor old_tensor_value_0 = ppoPolicy.Forward(old_input.GetTensor());
            at::Tensor old_probabs_0 = torch::softmax(old_tensor_value_0, -1);

            at::Tensor action_tensor = torch::multinomial(old_probabs_0, 1);
            int initial_action = action_tensor.item<int>();
            at::Tensor old_prob = old_probabs_0[0][initial_action].detach();

            int random_action = 0;
            at::Tensor old_state_value = ppoValue.Forward(old_input.GetTensor()).detach();
            at::Tensor A = -old_state_value.detach();

            for (int step = 0; step < forwardSteps; step++){
                s = GetState(playerClone, enemyClone, map);
                TensorStruct input(s, map);
    
                at::Tensor current_tensor = ppoPolicy.Forward(input.GetTensor());
                at::Tensor current_probabs = torch::softmax(current_tensor, -1);
                if (step != 0){
                    at::Tensor action_tensor = torch::multinomial(current_probabs, 1);
                    random_action = action_tensor.item<int>();
                }else{
                    random_action = initial_action;
                }
                index_old_probabilities.push_back({s, random_action, current_probabs[0][random_action].detach(), A.detach()});

                actionT action = ppoPolicy.MapIndexToAction(playerClone, enemyClone, random_action);
                actionT enemy_action = ppoPolicy.MapIndexToAction(enemyClone, playerClone, random_action);

                at::Tensor reward_tensor = torch::tensor(playerClone.TakeAction(action), torch::dtype(torch::kFloat32));
                enemyClone.TakeAction(enemy_action);
                float reward = reward_tensor.item<float>();
                A += std::pow(gamma, step) * reward_tensor.detach();

                if (ShouldResetEnvironment(pl, en, map, reward)){
                    done = true;
                    break;
                }

            }
            int ppo_epochs = 1;
            for (int k = 0; k < ppo_epochs; k++){
                float policy_loss_sum = 0.0f;
                float value_loss_sum = 0.0f;
                
                policy_optimizer.zero_grad();
                value_optimizer.zero_grad();
                
                for (auto &entry : index_old_probabilities) {
                    State &s = std::get<0>(entry);

                    int action = std::get<1>(entry);
                    at::Tensor old_prob = std::get<2>(entry);
                    at::Tensor advantage = std::get<3>(entry);  // Add advantage to your tuple!
                    
                    TensorStruct input(s, map);
                    
                    // Get new probability from updated policy
                    at::Tensor new_tensor = ppoPolicy.Forward(input.GetTensor());
                    at::Tensor probabs_new = torch::softmax(new_tensor, -1);
                    at::Tensor new_prob = probabs_new[0][action];
                    at::Tensor ratio = new_prob / old_prob;
                    
                    // PPO clipped loss
                    at::Tensor surr1 = ratio * advantage;
                    at::Tensor surr2 = torch::clamp(ratio, 1.0f - ppoEpsilon, 1.0f + ppoEpsilon) * advantage;
                    at::Tensor policy_loss = -torch::min(surr1, surr2);
                    
                    // Value loss - compute current value estimate
                    at::Tensor current_value = ppoValue.Forward(input.GetTensor());
                    at::Tensor value_target = advantage;  // Or use returns if you computed them
                    at::Tensor value_loss = torch::mse_loss(current_value, value_target.detach());

                    
                    // Backward on individual losses (they accumulate gradients)
                    policy_loss.backward(torch::ones_like(policy_loss), true);  // retain_graph=true
                    value_loss.backward(torch::ones_like(value_loss), true);

                    torch::nn::utils::clip_grad_norm_(ppoPolicy.parameters(), 0.5);
                    torch::nn::utils::clip_grad_norm_(ppoValue.parameters(), 0.5);
                
                    
                    // Track for logging
                    policy_loss_sum += policy_loss.item<float>();
                    value_loss_sum += value_loss.item<float>();
                }
                
                // Step optimizers once after all gradients accumulated
                policy_optimizer.step();
                value_optimizer.step();
                
                std::cout << "Epoch " << k << " | Policy Loss: " << policy_loss_sum 
                          << " | Value Loss: " << value_loss_sum << std::endl;
                file << policy_loss_sum << "\n";
            }            
            at::Tensor logits = ppoPolicy.Forward(old_input.GetTensor());
            at::Tensor probs_ac = torch::softmax(logits, -1); // shape: [1, num_actions]
            int best_action = probs_ac.argmax(-1).item<int>(); // picks the action with highest probability
            actionT action = ppoPolicy.MapIndexToAction(pl, en, best_action);
            actionT enemy_action = ppoPolicy.MapIndexToAction(en, pl, best_action);
            pl.TakeAction(action);
            en.TakeAction(enemy_action);
            State s1 = GetState(pl, en, map);
            for (auto& g : pl.units){
                std::cout<<g->coordinate.x << " "<<g->coordinate.y<<std::endl;
            }
            //ShowInMap(s1);
        }
    }
    file.close();
    ppoPolicy.SaveModel("ppo_policy");
}

void RlManager::ShowInMap(State& state){
    int a;
    std::string dqn_string = "yes";
    std::string ppo_string = "no";
    win.Render(state.playerUnits, state.enemyUnits, state.playerStructs,
    state.enemyStructs, dqn_string, ppo_string);
}

void RlManager::TrainDQN(Player &pl, Player &en, Map &map) {
    float updateRate = 0.005;

    for (int i = 0; i < episodeNumber; i++)
    {
        State currState = GetState(pl, en, map);
        for (int j = 0; j < 1000; j++)
        {

            auto selectedAction =
                policyNet.SelectAction(pl, en, map, currState, epsilon);
            float reward = pl.TakeAction(std::get<0>(selectedAction));
            State nextState = GetState(pl, en, map);
            if (ShouldResetEnvironment(pl, en, map, reward))
                break;
            Transition trans(currState, std::get<0>(selectedAction), nextState,
                             reward, std::get<1>(selectedAction));
            AddExperience(trans);
            selectedAction = policyNet.SelectAction(pl, en, map, nextState, epsilon);
            reward = en.TakeAction(std::get<0>(selectedAction));
            State nextNextState = GetState(pl, en, map);
            if (ShouldResetEnvironment(en, pl, map, reward)) // change order of pl, en to en, pl becasue en takes action here
                break;
            Transition trans1(nextState, std::get<0>(selectedAction), nextNextState,
                              reward, std::get<1>(selectedAction));
            AddExperience(trans1);
            OptimizeDQN(map);
            if (epsilon - epsilonDecay > 0)
                epsilon -= epsilonDecay;
        }
        // auto target_net_dict = targetNet.state_dict();
        // for (auto [key, val] : target_net_disct){
        //     target_net_dict = policyNet.state_dict()[key] * updateRate + (val * (1
        //     - updateRate));
        // }
        // targetNet.load_state_dict(target_net_dict);
    }
    SaveMemoryAsBinary();
    policyNet.SaveModel();
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

void RlManager::SaveMemoryAsString() {
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

void RlManager::LoadMemoryAsString() {
    std::ifstream file(memory_file);
    if (!file.is_open()) {
        std::cout << "String replay file couldn't be opened.\n";
        return;
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    file.close();
    Transition trans;
    trans = trans.Deserialize(fileContents);
    AddExperience(trans);
}

void RlManager::SaveMemoryAsBinary() {
    std::vector<binary> data_to_save;
    std::ofstream file;
    file.open(memory_file_binary, std::ios::binary);
    data_to_save.reserve(memory.size() * 1024);

    // Accumulate all serialized vectors
    for (int i = 0; i < memory.size(); i++) {
        std::vector<binary> data = memory[i].SerializeBinary();
        data_to_save.insert(data_to_save.end(), data.begin(), data.end());
    }
    file.write(reinterpret_cast<char*>(data_to_save.data()), data_to_save.size() * sizeof(binary));

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
