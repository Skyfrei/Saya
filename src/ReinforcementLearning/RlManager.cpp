#include "RlManager.h"

#include "Reward.h"
#include <chrono>
#include <fstream>
#include <random>
#include <tuple>
#include "Tensor.h"
#include <algorithm>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

std::string get_random_model(const std::string& directory) {
    std::vector<std::string> models;
    
    if (fs::exists(directory) && fs::is_directory(directory)) {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string pathWithoutExt = (entry.path().parent_path() / entry.path().stem()).string();
                models.push_back(pathWithoutExt);
            }
        }
    }

    static std::mt19937 gen(std::random_device{}()); 
    std::uniform_int_distribution<> dist(0, (int)models.size() - 1);

    return models[dist(gen)];
}

std::string get_latest_model(const std::string& directory_path, std::string file_start, std::string ext) {
    std::string latest_file = "";

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            
            if (filename.find(file_start) != std::string::npos && 
                filename.find(ext) != std::string::npos) {
                if (filename > latest_file) {
                    latest_file = filename;
                }
            }
        }
    }
    latest_file = latest_file.substr(0, latest_file.find("." + ext));
    std::string full_path = directory_path + latest_file;
    return full_path;
}

RlManager::RlManager() : win(Vec2(1000, 1000)) {
}
actionT RlManager::GetActionPPO(Player& pl, Player& en, Map& map){
    torch::NoGradGuard no_grad; 
    State s = GetState(pl, en, map);

    TensorStruct tensorStruct(s, map);
    
    auto output = ppoPolicy.Forward(tensorStruct.GetTensor());
    torch::Tensor mask = GetMask(pl, en, ppoPolicy.GetOutputSize()); 

    at::Tensor masked_output = output.masked_fill(mask == 0, -1e10);
    auto probs = torch::softmax(masked_output, 1);
    //int action_idx = probs.argmax(1).item<int>();
    //actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);

    ShowInMap(pl, en, map, probs);
    int k = std::min(50, (int)probs.size(1));
    auto topk_res = probs.topk(k, 1);
    auto top_probs = std::get<0>(topk_res);  
    auto top_indices = std::get<1>(topk_res);

    int sample_idx = torch::multinomial(top_probs, 1).item<int>();
    int action_idx = top_indices[0][sample_idx].item<int>();
    actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);

    return action;
}

actionT RlManager::GetActionPPOEnemy(Player& en, Player& pl, Map& map){
    torch::NoGradGuard no_grad; 
    State s = GetState(en, pl, map);

    TensorStruct tensorStruct(s, map);
    
    auto output = enemyPPO.Forward(tensorStruct.GetTensor());
    torch::Tensor mask = GetMask(en, pl, enemyPPO.GetOutputSize()); 

    at::Tensor masked_output = output.masked_fill(mask == 0, -1e10);
    auto probs = torch::softmax(masked_output, 1);
    //int action_idx = probs.argmax(1).item<int>();
    //actionT action = enemyPPO.MapIndexToAction(en, pl, action_idx);

    int k = std::min(50, (int)probs.size(1));
    auto topk_res = probs.topk(k, 1);
    auto top_probs = std::get<0>(topk_res);  
    auto top_indices = std::get<1>(topk_res);

    int sample_idx = torch::multinomial(top_probs, 1).item<int>();
    int action_idx = top_indices[0][sample_idx].item<int>();
    actionT action = enemyPPO.MapIndexToAction(en, pl, action_idx);
    return action;
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
    enemyPPO.Initialize(map, s);
    if (pl.side == PLAYER)
        enemyPPO.LoadModel(get_latest_model("models/enemy_models_ppo/", "ppo_policy-", "pt"));
    else
        enemyPPO.LoadModel(get_latest_model("models/player_model_ppo/", "ppo_policy-", "pt"));

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



bool RlManager::ShouldResetEnvironment(Player &pl, Player &en, Map &map) {
    const int hallCost = 590;
    auto Reset = [&](Player& p){
        bool hasPeasant = p.HasUnit(PEASANT);
        bool hasHall = p.HasStructure(HALL);

        if (!hasPeasant && !hasHall) return true;
        if (!hasPeasant && p.gold < 55) return true;
        if (hasPeasant && p.gold < hallCost && !hasHall) return true;
    
        return false;
    };

    if (Reset(pl) || Reset(en)){
        return true;
    }

    return false;
}

void count_action(std::unordered_map<std::string, int>& m, actionT& act){
    if (std::holds_alternative<MoveAction>(act))
        m["move"]++;
    else if (std::holds_alternative<AttackAction>(act))
        m["attack"]++;
    else if (std::holds_alternative<BuildAction>(act))
        m["build"]++;
    else if (std::holds_alternative<FarmGoldAction>(act))
        m["farm"]++;
    else if (std::holds_alternative<RecruitAction>(act))
        m["recruit"]++;
    else
        m["empty"]++;
}

at::Tensor RlManager::GetMask(Player &pl, Player& en, int outputSize){
    auto mask = torch::ones({1, outputSize}, torch::kFloat32);
    int moveEnd = ppoPolicy.moveAction;
    int attackEnd = ppoPolicy.attackAction;
    int buildEnd = ppoPolicy.buildAction;
    int farmEnd = ppoPolicy.farmAction;
    int recruitEnd = ppoPolicy.recruitAction;
    int recruitStart = farmEnd;
    int attackStride = (MAX_STRUCTS + MAX_UNITS);
    int buildStride = NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE;
    int farmStride = MAP_SIZE * MAP_SIZE * HALL_INDEX_IN_STRCTS;

    // Move
    for (int u = 0; u < MAX_UNITS; u++) {
        if (u < pl.units.size() && pl.units[u]->GetActionQueueSize() == 0) {
            int unitMoveStart = u * MAP_SIZE * MAP_SIZE;
            mask.narrow(1, unitMoveStart, MAP_SIZE * MAP_SIZE).fill_(1.0f); 

            int curX = pl.units[u]->coordinate.x; 
            int curY = pl.units[u]->coordinate.y; 
            mask[0][unitMoveStart + (curX * MAP_SIZE) + curY] = 0.0f;
        }
    }

    // Attack
    for (int u = 0; u < MAX_UNITS; u++) {
        int unitAttackStart = moveEnd + (u * attackStride);
        if (u < pl.units.size() && pl.units[u]->GetActionQueueSize() == 0) {
            for (int t = 0; t < attackStride; t++) {
                bool targetExists = false;
                if (t < MAX_STRUCTS) {
                    if (t < en.structures.size()) targetExists = true;
                } else {
                    int unitTargetIdx = t - MAX_STRUCTS;
                    if (unitTargetIdx < en.units.size()) targetExists = true;
                }

                if (targetExists) {
                    mask[0][unitAttackStart + t] = 1.0f; 
                }
            }
        }
    }

    // Build
    if (pl.gold >= 100) {
        for (int u = 0; u < PEASANT_INDEX_IN_UNITS; u++) {
            if (u < pl.units.size() && pl.units[u]->is == PEASANT && pl.units[u]->GetActionQueueSize() == 0) {
                int startIdx = attackEnd + (u * buildStride);
                mask.narrow(1, startIdx, buildStride).fill_(1.0f);
            }
        }
    }

    // Farm
    for (int u = 0; u < PEASANT_INDEX_IN_UNITS; u++) {
        if (u < pl.units.size() && pl.units[u]->is == PEASANT && pl.units[u]->GetActionQueueSize() == 0) {
            for (int h = 0; h < HALL_INDEX_IN_STRCTS; h++) {
                if (h < pl.structures.size() && pl.structures[h]->is == HALL) {
                    int startIdx = buildEnd + (u * MAP_SIZE * MAP_SIZE * HALL_INDEX_IN_STRCTS) + (h * MAP_SIZE * MAP_SIZE);
                    for (int x = 0; x < MAP_SIZE; x++) {
                        for (int y = 0; y < MAP_SIZE; y++) {
                            if (pl.map.GetTerrainAtCoordinate(Vec2(x, y)).resourceLeft > 0) {
                                mask[0][startIdx + (x * MAP_SIZE + y)] = 1.0f;
                            }
                        }
                    }
                }
            }
        }
    }
    // Recruit
    for (int offset = 0; offset < (recruitEnd - farmEnd); offset++) {
        int unitTypeInt = offset / MAX_STRUCTS;
        int structureIndex = offset % MAX_STRUCTS;
        if (structureIndex >= pl.structures.size()) continue;

        Structure* stru = pl.structures[structureIndex].get();
        UnitType unitType = static_cast<UnitType>(unitTypeInt);

        if (unitType == PEASANT) {
            if (stru->is == HALL && pl.gold >= 55) 
                mask[0][recruitStart + offset] = 1.0f;
        }
        else if (unitType == FOOTMAN) {
            if (stru->is == BARRACK && pl.gold >= 75) 
                mask[0][recruitStart + offset] = 1.0f;
        }
    }    
    return mask;
}
std::string get_current_time(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%m-%d_%H-%M");

    return ss.str();
}



void RlManager::TrainPPO(Player &pl, Player &en, Map &map){
    torch::optim::AdamW policy_optimizer(
        ppoPolicy.parameters(), torch::optim::AdamWOptions(3e-4).weight_decay(1e-4));
    torch::optim::AdamW value_optimizer(
        ppoValue.parameters(), torch::optim::AdamWOptions(3e-4).weight_decay(1e-4));

    if (pl.side == PLAYER){
        ppoPolicy.LoadModel(get_latest_model("models/player_model_ppo/", "ppo_policy-", "pt"));
        ppoValue.LoadModel(get_latest_model("models/player_value/", "ppo_policy-", "pt"));
    }
    else{
        ppoPolicy.LoadModel(get_latest_model("models/enemy_models_ppo/", "ppo_policy-", "pt"));
        ppoValue.LoadModel(get_latest_model("models/enemy_value/", "ppo_policy-", "pt"));
    }

    double start_lr = 6e-4; // A bit "bigger" to start
    double end_lr = 1e-4;


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, ppoPolicy.layer3->options.out_features() - 1);

    bool done = false;
    
    for (int i = 0; i < episodeNumber; i++){
        if (i % 3 == 0 && i != 0){
            if (pl.side == PLAYER)
                enemyPPO.LoadModel(get_random_model("models/enemy_models_ppo/"));
            else
                enemyPPO.LoadModel(get_random_model("models/player_model_ppo/"));
        }
        // Updating optimizers
        double progress = static_cast<double>(i) / episodeNumber;
        double current_lr = start_lr + progress * (end_lr - start_lr);

        for (auto &group : policy_optimizer.param_groups()) {
            static_cast<torch::optim::AdamWOptions &>(group.options()).lr(current_lr);
        }
        for (auto &group : value_optimizer.param_groups()) {
            static_cast<torch::optim::AdamWOptions &>(group.options()).lr(current_lr);
        }

        done = false;
        double fullSteps = 0;
        std::unordered_map<std::string, int> action_count_map = {{"move", 0}, {"attack", 0}, {"build", 0}, {"farm", 0}, {"recruit", 0}};
        while (!done){
            float episode_reward = 0.0f;
            std::vector<std::tuple<TensorStruct, int, float, at::Tensor, at::Tensor, bool, at::Tensor>> trajectory_buffer;

            for (int t= 0; t < forwardSteps; t++){

                State s = GetState(pl, en, map);
                TensorStruct input_tensor(s, map);
                at::Tensor state_value = ppoValue.Forward(input_tensor.GetTensor()).clone().detach();
                
                at::Tensor output = ppoPolicy.Forward(input_tensor.GetTensor());
                at::Tensor mask = GetMask(pl, en, ppoPolicy.GetOutputSize());

                auto lamba_output = [](const at::Tensor& output, const at::Tensor& mask){
                    at::Tensor masked_output = output.masked_fill(mask == 0, -1e10);
                    return torch::softmax(masked_output, -1);
                };

                at::Tensor output_soft = lamba_output(output, mask); 
                at::Tensor action_tensor = torch::multinomial(output_soft, 1);
                int action_index = action_tensor.item<int>();
                actionT action = ppoPolicy.MapIndexToAction(pl, en, action_index);
                count_action(action_count_map, action);
                State s1 = GetState(en, pl, map);

                TensorStruct input_tensor1(s1, map);
                at::Tensor output1 = enemyPPO.Forward(input_tensor1.GetTensor()).clone().detach();
                
                at::Tensor mask1 = GetMask(en, pl, enemyPPO.GetOutputSize());
                at::Tensor output_soft1 = lamba_output(output1, mask1);

                at::Tensor action_tensor1 = torch::multinomial(output_soft1, 1);
                int action_index1 = action_tensor1.item<int>();
                actionT enemy_action = enemyPPO.MapIndexToAction(en, pl, action_index1);

                at::Tensor reward_tensor = torch::tensor(pl.TakeAction(action), torch::dtype(torch::kFloat32));
                en.TakeAction(enemy_action);

                float reward = reward_tensor.item<float>() - 0.1f; // 0.1 here is a time penalty
                episode_reward += reward;
                /* doing this for dqn experiecne */ 
                Transition trans(s, action, s1, reward, action_index, done);
                AddExperience(trans);
                /* doing this for dqn experiecne */ 


                ShowInMap(pl, en, map, output_soft);
                bool envDone = ShouldResetEnvironment(pl, en, map);
                bool timeDone = (fullSteps >= 5000);
                done = envDone || timeDone;

               if (envDone) {
                    const int hallCost = 590; 
                    bool plHasPeasant = pl.HasUnit(PEASANT);
                    bool plHasHall = pl.HasStructure(HALL);
                    bool plLost = (!plHasPeasant && !plHasHall) || 
                                  (!plHasPeasant && pl.gold < 55) || 
                                  (plHasPeasant && pl.gold < hallCost && !plHasHall);

                    bool enLost = (!en.HasUnit(PEASANT) && !en.HasStructure(HALL)); 
                
                    if (plLost) {
                        reward -= 500.0f;
                    } else if (enLost){
                        reward += 500.0f;
                    }
                }

                trajectory_buffer.push_back({input_tensor, action_index, reward, state_value, output_soft[0][action_index].clone().detach(), done, mask.clone().detach()});
                if (done)
                    break;

                fullSteps ++;
            }

            for (const auto& [action, count] : action_count_map) {
                std::cout << action << ": " << count << " | ";
            }
            std::cout<< "p.gold: "<< pl.gold<< "|en.gold: "<< en.gold << "|pun: "<<pl.units.size()<< "|eun: "<<en.units.size() << std::endl;

            at::Tensor gae = torch::tensor(0.0f, torch::kFloat32); // was 0 
            std::vector<at::Tensor> advantages_buffer(trajectory_buffer.size());
            std::vector<at::Tensor> returns_buffer(trajectory_buffer.size());
            
            //bootstrap
            at::Tensor new_value;
            if (std::get<5>(trajectory_buffer[trajectory_buffer.size() - 1]) == 1){
                new_value = torch::tensor(0.0f, torch::kFloat32);
            }else{
                State s = GetState(pl, en, map);
                TensorStruct input_tensor(s, map);
                new_value = ppoValue.Forward(input_tensor.GetTensor()).clone().detach();
            }

            for (int t = trajectory_buffer.size() - 1; t >= 0; t--){
                float reward = std::get<2>(trajectory_buffer[t]);
                at::Tensor value = std::get<3>(trajectory_buffer[t]);
                at::Tensor delta = reward + gamma * new_value - value; 
                bool currDone = std::get<5>(trajectory_buffer[t]);
                new_value = value;
                gae = delta + gamma * 0.95 * (1 - currDone) * gae;
                advantages_buffer[t] = gae.clone().detach();
                returns_buffer[t] = (gae + value).clone().detach(); 
            }
            at::Tensor adv_stack = torch::stack(advantages_buffer);
            at::Tensor adv_mean = adv_stack.mean();
            at::Tensor adv_std = adv_stack.std() + 1e-8f;
            for (auto& adv : advantages_buffer)
                adv = ((adv - adv_mean) / adv_std).clone().detach();

            std::cout << "Episode: " << i <<" | Fullsteps: "<< fullSteps << " | Total Reward: " << episode_reward << std::endl;
            /** just stacking the tensors **/
            std::vector<at::Tensor> b_states, b_actions, b_old_probs, b_advs, b_returns, b_masks;
            for (auto& t : trajectory_buffer) {
                b_states.push_back(std::get<0>(t).GetTensor());
                b_actions.push_back(torch::tensor(std::get<1>(t), torch::kLong)); // Actions must be Long
                b_old_probs.push_back(std::get<4>(t));
                b_masks.push_back(std::get<6>(t));
            }
            at::Tensor batch_states = torch::cat(b_states, 0).detach(); 
            at::Tensor batch_actions = torch::stack(b_actions).detach();
            at::Tensor batch_masks = torch::cat(b_masks, 0).detach();
            at::Tensor batch_old_probs = torch::stack(b_old_probs).detach().view({-1});
            at::Tensor batch_advs = torch::stack(advantages_buffer).detach().view({-1});
            at::Tensor batch_returns = torch::stack(returns_buffer).detach().view({-1});
            /** just stacking the tensors **/
            for (int k = 0; k < 5; k++){
                policy_optimizer.zero_grad();
                value_optimizer.zero_grad();
                // A. Forward pass on the ENTIRE batch at once
                at::Tensor all_policy_output = ppoPolicy.Forward(batch_states); 
                at::Tensor all_values = ppoValue.Forward(batch_states).squeeze();

                // B. Apply Masking Vectorized
                at::Tensor masked_output = all_policy_output.masked_fill(batch_masks == 0, -1e10);
                at::Tensor new_probs_dist = torch::softmax(masked_output, -1);

                // C. Select the probabilities of the actions we actually took
                // gather selects elements from new_probs_dist using batch_actions indices
                at::Tensor new_probs = new_probs_dist.gather(1, batch_actions.unsqueeze(1)).squeeze();

                // D. Calculate Ratios
                at::Tensor ratio = torch::exp(torch::log(new_probs + 1e-10) - torch::log(batch_old_probs + 1e-10));

                // E. Policy Loss
                at::Tensor surr1 = ratio * batch_advs;
                at::Tensor surr2 = torch::clamp(ratio, 1.0f - ppoEpsilon, 1.0f + ppoEpsilon) * batch_advs;
                at::Tensor policy_loss = -torch::min(surr1, surr2).mean(); // Mean over the batch

                // F. Value Loss
                at::Tensor value_loss = torch::mse_loss(all_values, batch_returns);

                // G. Entropy
                at::Tensor entropy = -(new_probs_dist * torch::log(new_probs_dist + 1e-10)).sum(-1).mean();

                // H. Total Loss & Backprop
                float entropy_coeff = std::max(0.02, 0.05 - (static_cast<double>(i) / episodeNumber) * (0.05 - 0.02));
                at::Tensor total_loss = policy_loss + value_loss - (entropy_coeff * entropy);

                total_loss.backward();
                
                torch::nn::utils::clip_grad_norm_(ppoPolicy.parameters(), 0.5f);
                torch::nn::utils::clip_grad_norm_(ppoValue.parameters(), 0.5f);
                
                policy_optimizer.step();
                value_optimizer.step();
                std::cout << "  Policy Loss: " << policy_loss.item<float>() 
                          << " | Value Loss: " << value_loss.item<float>() 
                          << std::endl;
                }
            if (done) {
                map.Reset();
                pl.Reset(pl.side);
                en.Reset(en.side);
            }
        }
        if ((i % 25 == 0 && i != 0) || i == episodeNumber - 1 ){
            SaveMemoryAsBinary();
            if (pl.side == PLAYER){
                ppoPolicy.SaveModel("models/player_model_ppo/ppo_policy-" + get_current_time());
                ppoValue.SaveModel("models/player_value/ppo_policy-" + get_current_time());
            }
            else{
                ppoPolicy.SaveModel("models/enemy_models_ppo/ppo_policy-" + get_current_time());
                ppoValue.SaveModel("models/enemy_value/ppo_policy-" + get_current_time());
            }
        }
    }
}

void RlManager::ShowInMap(Player& pl, Player& en, Map& m, at::Tensor& tensor){
    std::string dqn_string = "yes";
    std::string ppo_string = "";

    int k = std::min(10, (int)tensor.size(1));
    auto topk_res = tensor.topk(k, 1);
    auto top_values = std::get<0>(topk_res);
    auto top_indices = std::get<1>(topk_res);

    for (int i = 0; i < k; ++i) {
        int action_idx = top_indices[0][i].item<int>();
        float conf_value = top_values[0][i].item<float>(); 
        actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f%%", conf_value * 100.0f);
        ppo_string += std::to_string(i + 1) + ". [" + std::string(buf) + "] ";

        if (std::holds_alternative<MoveAction>(action)) {
            auto& act = std::get<MoveAction>(action);
            ppo_string += "Move unit (" + std::to_string(act.unit->coordinate.x) + ", "
                + std::to_string(act.unit->coordinate.y) + ") to (" + 
                std::to_string(act.destCoord.x) + ", " + std::to_string(act.destCoord.y) + ")";
            
        } else if (std::holds_alternative<FarmGoldAction>(action)) {
            auto& act = std::get<FarmGoldAction>(action);
            ppo_string += "Farm with unit (" + std::to_string(act.peasant->coordinate.x) + "," +
                std::to_string(act.peasant->coordinate.y) + ") " + "at (" + std::to_string(act.destCoord.x)
                + ", " + std::to_string(act.destCoord.y) + ")";
            
        } else if (std::holds_alternative<BuildAction>(action)) {
            auto& act = std::get<BuildAction>(action);
            std::string bType = "";
            switch(act.struType){
                 case HALL:
                    bType = "Hall";
                    break;
                case BARRACK:
                    bType = "Barrack";
                    break;
                case FARM:
                    bType = "Farm";
                    break;
                default:
                    break;
            }
            ppo_string += "Build at (" + std::to_string(act.coordinate.x) + ", " + std::to_string(act.coordinate.y) + ") "
                        + bType;
            
        } else if (std::holds_alternative<AttackAction>(action)) {
            auto& act = std::get<AttackAction>(action);
            ppo_string += "Attack (" + std::to_string(act.object->coordinate.x) + ", " + std::to_string(act.object->coordinate.y) + ")"; 
            
        } else if (std::holds_alternative<RecruitAction>(action)) {
            auto& act = std::get<RecruitAction>(action);
            std::string uType = "";
            switch(act.unitType){
                case FOOTMAN:
                    uType = "Footman";
                    break;
                case PEASANT:
                    uType = "Peasant";
                    break;
                default:
                    break;
            }
            ppo_string += "Recruit type: " + uType; 
        } else if (std::holds_alternative<EmptyAction>(action)) {
            ppo_string += "Empty action";
        }
        ppo_string += "\n";
    }
    win.Render(pl, en, m, dqn_string, ppo_string);
}

void RlManager::TrainDQN(Player &pl, Player &en, Map &map) {
    // float updateRate = 0.005;rlmana
    float tau = 0.005; 
    torch::optim::AdamW optimizer(
        policyNet.parameters(), torch::optim::AdamWOptions(0.01).weight_decay(1e-4));

    LoadMemoryAsBinary();
    for (int i = 0; i < episodeNumber; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            State currState = GetState(pl, en, map);

            auto selectedAction =
                policyNet.SelectAction(pl, en, map, currState, epsilon);

            float reward = pl.TakeAction(std::get<0>(selectedAction));
            State nextState = GetState(pl, en, map);
            bool done = ShouldResetEnvironment(pl, en, map);

            Transition trans(currState, std::get<0>(selectedAction), nextState,
                             reward, std::get<1>(selectedAction), done);
            AddExperience(trans);

            selectedAction = policyNet.SelectAction(en, pl, map, nextState, epsilon);
            en.TakeAction(std::get<0>(selectedAction));

            OptimizeDQN(map, optimizer);
            epsilon = std::max(0.05f, epsilon - epsilonDecay);

            if (done)
                break;
        }
        torch::NoGradGuard no_grad;
        auto target_params = targetNet.parameters();
        auto policy_params = policyNet.parameters();
        for (size_t k = 0; k < target_params.size(); k++) {
            target_params[k].copy_(target_params[k] * (1.0 - tau) + policy_params[k] * tau);
        }
    }
    SaveMemoryAsBinary();
    policyNet.SaveModel();
}

void RlManager::OptimizeDQN(Map &map, torch::optim::AdamW& optimizer) {
    int batch_size = 512;
    if (memory.size() < batch_size)
        return;

    std::random_device dev;
    std::mt19937 rng(dev());

    std::deque<Transition> samples;
    std::sample(memory.begin(), memory.end(), std::back_inserter(samples),
                batch_size, rng);

    std::vector<torch::Tensor> state_batch;
    std::vector<torch::Tensor> state_action;
    std::vector<torch::Tensor> next_state_batch;
    std::vector<torch::Tensor> reward_batch;
    std::vector<torch::Tensor> done_batch;
    done_batch.reserve(batch_size);

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
        done_batch.push_back(torch::tensor({trans.done ? 0.0f : 1.0f}, torch::kFloat32));
    }

    torch::Tensor tensor_states = torch::cat(state_batch);
    torch::Tensor tensor_actions = torch::cat(state_action).unsqueeze(1);
    torch::Tensor tensor_rewards = torch::cat(reward_batch).unsqueeze(1);
    torch::Tensor tensor_dones = torch::cat(done_batch).unsqueeze(1);

    torch::Tensor q_values =
        policyNet.Forward(tensor_states).gather(1, tensor_actions);

    torch::Tensor tensor_next_states = torch::cat(next_state_batch);
    torch::Tensor q_next_values;
    {
        torch::NoGradGuard no_grad;
        q_next_values = tensor_rewards + (gamma * q_next_values * tensor_dones);
    }
    q_next_values = (q_next_values * gamma) + tensor_rewards;

    auto criterion = torch::nn::SmoothL1Loss();
    auto loss = criterion(q_values, q_next_values);

    std::cout << "Loss: " << loss.item<float>() << std::endl;
    optimizer.zero_grad();
    loss.backward();
    torch::nn::utils::clip_grad_value_(policyNet.parameters(), 100);
    optimizer.step();
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
    std::string memory_file_binary = "models/player_dqn_experience/binary-" + get_current_time() + ".bay";
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
