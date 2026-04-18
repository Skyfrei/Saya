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
#include <ATen/Parallel.h>

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

    // NEW PROTECTIVE GUARD:
    if (models.empty()) {
        return ""; 
    }

    static std::mt19937 gen(std::random_device{}()); 
    std::uniform_int_distribution<> dist(0, (int)models.size() - 1);

    return models[dist(gen)];
}

std::string get_latest_model(const std::string& directory_path, std::string file_start, std::string ext) {
    if (!fs::exists(directory_path)) {
        fs::create_directories(directory_path);
        return ""; 
    }

    std::string latest_file = "";
    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(file_start) != std::string::npos && 
                filename.find(ext) != std::string::npos) {
                if (filename > latest_file) latest_file = filename;
            }
        }
    }

    if (latest_file.empty()) return "";

    latest_file = latest_file.substr(0, latest_file.find("." + ext));
    return directory_path + latest_file;
}


RlManager::RlManager() : win(Vec2(1000, 1000)) {
  at::set_num_threads(1);
}

actionT RlManager::GetActionPPO(Player& pl, Player& en, Map& map){
    torch::NoGradGuard no_grad; 
    State s = GetState(pl, en, map);

    TensorStruct tensorStruct(s, map);
    
    auto output = ppoPolicy.Forward(tensorStruct.GetTensor());
    torch::Tensor mask = GetMask(pl, en, ppoPolicy.GetOutputSize()); 

    at::Tensor masked_output = output.masked_fill(mask == 0, -1e10);
    auto probs = torch::softmax(masked_output, 1);

    ShowInMap(pl, en, map, probs);
    int k = std::min(10, (int)probs.size(1));
    auto topk_res = probs.topk(k, 1);
    int sample_idx = torch::multinomial(std::get<0>(topk_res), 1).item<int>();
    int action_idx = std::get<1>(topk_res)[0][sample_idx].item<int>();
    actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);

    return action;
}

actionT RlManager::GetActionDQN(Player& pl, Player& en, Map& map) {
    torch::NoGradGuard no_grad;
    State s = GetState(pl, en, map);
    at::Tensor s_tensor = TensorStruct(s, map).GetTensor();
    at::Tensor mask = GetMask(pl, en, policyNet.actionSize);
    
    at::Tensor q_values = policyNet.Forward(s_tensor);
    at::Tensor masked_q = q_values.masked_fill(mask == 0, -1e10);

    float temperature = 0.8f; 
    at::Tensor probs = torch::softmax(masked_q / temperature, 1);
    
    int act_idx = torch::multinomial(probs, 1).item<int>();
    
    //ShowInMap(pl, en, map, probs, "dqn");
    
    return policyNet.MapIndexToAction(pl, en, act_idx);
}

actionT RlManager::GetActionDQNEnemy(Player& en, Player& pl, Map& map) {
    torch::NoGradGuard no_grad; 
    State s = GetState(en, pl, map);
    at::Tensor s_tensor = TensorStruct(s, map).GetTensor();
    
    auto q_values = targetNet.Forward(s_tensor);
    at::Tensor mask = GetMask(en, pl, targetNet.actionSize); 
    at::Tensor masked_q = q_values.masked_fill(mask == 0, -1e10);

    float temperature = 0.8f;
    at::Tensor probs = torch::softmax(masked_q / temperature, 1);
    int action_idx = torch::multinomial(probs, 1).item<int>();

    return targetNet.MapIndexToAction(en, pl, action_idx);
}

actionT RlManager::GetActionPPOEnemy(Player& en, Player& pl, Map& map){
    torch::NoGradGuard no_grad; 
    State s = GetState(en, pl, map);

    TensorStruct tensorStruct(s, map);
    
    auto output = enemyPPO.Forward(tensorStruct.GetTensor());
    torch::Tensor mask = GetMask(en, pl, enemyPPO.GetOutputSize()); 

    at::Tensor masked_output = output.masked_fill(mask == 0, -1e10);
    auto probs = torch::softmax(masked_output, 1);

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

    std::string enemy_path = (pl.side == PLAYER) ? 
        get_latest_model("models/enemy_models_dqn/", "dqn_policy-", "pt") : 
        get_latest_model("models/player_models_dqn/", "dqn_policy-", "pt");

    if (!enemy_path.empty()) {
        targetNet.LoadModel(enemy_path);
    } else {
        std::cout << "No enemy model found. Training against random initialization." << std::endl;
    }

}

void RlManager::InitializePPO(Player &pl, Player &en, Map &map){
    State s = GetState(pl, en, map);
    ppoPolicy.Initialize(map, s);
    enemyPPO.Initialize(map, s);

    std::string enemy_path = (pl.side == PLAYER) ? 
        get_latest_model("models/enemy_models_ppo/", "ppo_policy-", "pt") : 
        get_latest_model("models/player_model_ppo/", "ppo_policy-", "pt");

    if (!enemy_path.empty()) {
        enemyPPO.LoadModel(enemy_path);
    } else {
        std::cout << "No enemy model found. Training against random initialization." << std::endl;
    }
    ppoValue.Initialize(ppoPolicy);
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
    std::vector<float> maskData(outputSize, 0.0f);
    
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
            for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
                maskData[unitMoveStart + i] = 1.0f; // Enable all moves
            }
            int curX = pl.units[u]->coordinate.x; 
            int curY = pl.units[u]->coordinate.y; 
            maskData[unitMoveStart + (curX * MAP_SIZE) + curY] = 0.0f; // Disable moving to self
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
                    maskData[unitAttackStart + t] = 1.0f; 
                }
            }
        }
    }

    // Build
    if (pl.gold >= 100) {
        for (int u = 0; u < PEASANT_INDEX_IN_UNITS; u++) {
            if (u < pl.units.size() && pl.units[u]->is == PEASANT && pl.units[u]->GetActionQueueSize() == 0) {
                int startIdx = attackEnd + (u * buildStride);
                for (int i = 0; i < buildStride; i++) {
                    maskData[startIdx + i] = 1.0f;
                }
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
                                maskData[startIdx + (x * MAP_SIZE + y)] = 1.0f;
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
                maskData[recruitStart + offset] = 1.0f;
        }
        else if (unitType == FOOTMAN) {
            if (stru->is == BARRACK && pl.gold >= 75) 
                maskData[recruitStart + offset] = 1.0f;
        }
    }    

    return torch::from_blob(maskData.data(), {1, outputSize}, torch::kFloat32).clone();
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

    std::string policy_path, value_path;
    if (pl.side == PLAYER) {
        policy_path = get_latest_model("models/player_model_ppo/", "ppo_policy-", "pt");
        value_path = get_latest_model("models/player_value/", "ppo_policy-", "pt");
    } else {
        policy_path = get_latest_model("models/enemy_models_ppo/", "ppo_policy-", "pt");
        value_path = get_latest_model("models/enemy_value/", "ppo_policy-", "pt");
    }

    if (!policy_path.empty()) ppoPolicy.LoadModel(policy_path);
    if (!value_path.empty()) ppoValue.LoadModel(value_path);

    double start_lr = 6e-4; // A bit "bigger" to start
    double end_lr = 1e-4;


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, ppoPolicy.layer3->options.out_features() - 1);

    bool done = false;
    
    for (int i = 0; i < episodeNumber; i++){
        if (i % 3 == 0 && i != 0){
            std::string random_enemy_path = (pl.side == PLAYER) ?
                get_random_model("models/enemy_models_ppo/") :
                get_random_model("models/player_model_ppo/");

            if (!random_enemy_path.empty()) {
                enemyPPO.LoadModel(random_enemy_path);
            }
        }        // Updating optimizers
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
        static bool is_paused = false;

        while (!done){
            float episode_reward = 0.0f;
            std::vector<std::tuple<TensorStruct, int, float, at::Tensor, at::Tensor, bool, at::Tensor>> trajectory_buffer;
            win.PollEvents(is_paused);
            while (is_paused) {
                win.PollEvents(is_paused);
                SDL_Delay(10);
            }

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
                float raw_reward = pl.TakeAction(action);

                State s1 = GetState(en, pl, map);

                TensorStruct input_tensor1(s1, map);
                at::Tensor output1 = enemyPPO.Forward(input_tensor1.GetTensor()).clone().detach();
                
                at::Tensor mask1 = GetMask(en, pl, enemyPPO.GetOutputSize());
                at::Tensor output_soft1 = lamba_output(output1, mask1);

                at::Tensor action_tensor1 = torch::multinomial(output_soft1, 1);
                int action_index1 = action_tensor1.item<int>();
                actionT enemy_action = enemyPPO.MapIndexToAction(en, pl, action_index1);

                en.TakeAction(enemy_action);

                float reward = raw_reward - 0.01f; // 0.1 here is a time penalty
                episode_reward += reward;

                if (fullSteps == 0 || (int)fullSteps % 20 == 0){ 
                  ShowInMap(pl, en, map, output_soft); 
                }
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
                        reward -= 5.0f;
                    } else if (enLost){
                        reward += 5.0f;
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
                bool currDone = std::get<5>(trajectory_buffer[t]);

                at::Tensor delta = reward + gamma * new_value * (1 - currDone) - value; 
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
                at::Tensor ratio = new_probs / (batch_old_probs + 1e-10);

                // E. Policy Loss
                at::Tensor surr1 = ratio * batch_advs;
                at::Tensor surr2 = torch::clamp(ratio, 1.0f - ppoEpsilon, 1.0f + ppoEpsilon) * batch_advs;
                at::Tensor policy_loss = -torch::min(surr1, surr2).mean(); // Mean over the batch

                // F. Value Loss
                at::Tensor value_loss = torch::nn::functional::smooth_l1_loss(all_values, batch_returns);

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

void RlManager::ShowInMap(Player& pl, Player& en, Map& m, at::Tensor& tensor, std::string mode) {
    std::string ui_string = "";

    int k = std::min(10, (int)tensor.size(1));
    auto topk_res = tensor.topk(k, 1);
    auto top_values = std::get<0>(topk_res);
    auto top_indices = std::get<1>(topk_res);

    for (int i = 0; i < k; ++i) {
        int action_idx = top_indices[0][i].item<int>();
        float conf_value = top_values[0][i].item<float>(); 
        
        // Check the string mode to map the action using the correct network
        actionT action = (mode == "dqn") ? policyNet.MapIndexToAction(pl, en, action_idx) : ppoPolicy.MapIndexToAction(pl, en, action_idx);
        
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f%%", conf_value * 100.0f);
        ui_string += std::to_string(i + 1) + ". [" + std::string(buf) + "] ";

        if (std::holds_alternative<MoveAction>(action)) {
            auto& act = std::get<MoveAction>(action);
            ui_string += "Move unit (" + std::to_string(act.unit->coordinate.x) + ", "
                + std::to_string(act.unit->coordinate.y) + ") to (" + 
                std::to_string(act.destCoord.x) + ", " + std::to_string(act.destCoord.y) + ")";
            
        } else if (std::holds_alternative<FarmGoldAction>(action)) {
            auto& act = std::get<FarmGoldAction>(action);
            ui_string += "Farm with unit (" + std::to_string(act.peasant->coordinate.x) + "," +
                std::to_string(act.peasant->coordinate.y) + ") " + "at (" + std::to_string(act.destCoord.x)
                + ", " + std::to_string(act.destCoord.y) + ")";
            
        } else if (std::holds_alternative<BuildAction>(action)) {
            auto& act = std::get<BuildAction>(action);
            std::string bType = "";
            switch(act.struType){
                 case HALL: bType = "Hall"; break;
                 case BARRACK: bType = "Barrack"; break;
                 case FARM: bType = "Farm"; break;
                 default: break;
            }
            ui_string += "Build at (" + std::to_string(act.coordinate.x) + ", " + std::to_string(act.coordinate.y) + ") " + bType;
            
        } else if (std::holds_alternative<AttackAction>(action)) {
            auto& act = std::get<AttackAction>(action);
            ui_string += "Attack (" + std::to_string(act.object->coordinate.x) + ", " + std::to_string(act.object->coordinate.y) + ")"; 
            
        } else if (std::holds_alternative<RecruitAction>(action)) {
            auto& act = std::get<RecruitAction>(action);
            std::string uType = "";
            switch(act.unitType){
                case FOOTMAN: uType = "Footman"; break;
                case PEASANT: uType = "Peasant"; break;
                default: break;
            }
            ui_string += "Recruit type: " + uType; 
        } else if (std::holds_alternative<EmptyAction>(action)) {
            ui_string += "Empty action";
        }
        ui_string += "\n";
    }
    
    // Render on the left (DQN) or right (PPO) based on the string mode
    if (mode == "dqn") {
        win.Render(pl, en, m, ui_string, "");
    } else {
        win.Render(pl, en, m, "yes", ui_string);
    }
}

void RlManager::TrainDQN(Player &pl, Player &en, Map &map) {
    float tau = 0.005; 
    torch::optim::AdamW optimizer(
        policyNet.parameters(), torch::optim::AdamWOptions(3e-4).weight_decay(1e-4));
    std::string policy_path, target_path;
    if (pl.side == PLAYER) {
        policy_path = get_latest_model("models/player_models_dqn/", "dqn_policy-", "pt");
        target_path = get_latest_model("models/enemy_models_dqn/", "dqn_policy-", "pt");
    } else {
        policy_path = get_latest_model("models/enemy_models_dqn/", "dqn_policy-", "pt");
        target_path = get_latest_model("models/player_models_dqn/", "dqn_policy-", "pt");
    }
    if (!policy_path.empty()) {
        policyNet.LoadModel(policy_path);
        std::cout << ">>> Resumed Policy from: " << policy_path << std::endl;
    }
    if (!target_path.empty()) {
        targetNet.LoadModel(target_path);
        std::cout << ">>> Resumed Target/Enemy from: " << target_path << std::endl;
    }

    LoadMemoryAsBinary();
    epsilonDecay = 0.95f / episodeNumber; 
    epsilon = 1.0f;

    for (int i = 0; i < episodeNumber; i++) {
        std::unordered_map<std::string, int> action_count_map = {
            {"move", 0}, {"attack", 0}, {"build", 0}, {"farm", 0}, {"recruit", 0}, {"empty", 0}
        };
        float total_reward = 0.0f;
        int steps = 0;
        bool done = false;
        bool is_paused = false;

        while (!done && steps < 5000) {
            win.PollEvents(is_paused);
            while (is_paused) { win.PollEvents(is_paused); SDL_Delay(10); }

            State currState = GetState(pl, en, map);
            torch::Tensor s_tensor = TensorStruct(currState, map).GetTensor().clone();
            at::Tensor mask = GetMask(pl, en, policyNet.actionSize); 

            int act_idx;
            at::Tensor q_values = policyNet.Forward(s_tensor);

            
            if (torch::rand({1}).item<float>() > epsilon) {
                at::Tensor masked_q = q_values.masked_fill(mask == 0, -1e10);
                act_idx = masked_q.argmax(1).item<int>();
            } else {
                at::Tensor valid_indices = torch::nonzero(mask.flatten() == 1).flatten();
                if (valid_indices.size(0) > 0) {
                    // Uniformly sample from valid indices (same logic as the enemy)
                    int sampled_pos = torch::randint(0, valid_indices.size(0), {}).item<long>();
                    act_idx = valid_indices[sampled_pos].item<int>();
                } else {
                    act_idx = 0; // Fallback 
                }
            }
            actionT p_act = policyNet.MapIndexToAction(pl, en, act_idx);
            count_action(action_count_map, p_act);
            float reward = pl.TakeAction(p_act);

            State enState = GetState(en, pl, map);
            at::Tensor en_s_tensor = TensorStruct(enState, map).GetTensor();
            at::Tensor en_mask = GetMask(en, pl, targetNet.actionSize);
            at::Tensor en_q = targetNet.Forward(en_s_tensor);
            int en_act_idx;
            
            if (torch::rand({1}).item<float>() > 0.2f) {
                en_act_idx = en_q.masked_fill(en_mask == 0, -1e10).argmax(1).item<int>();
            } else {
                at::Tensor en_valid = torch::nonzero(en_mask.flatten() == 1).flatten();
                en_act_idx = (en_valid.size(0) > 0) ? en_valid[torch::randint(0, en_valid.size(0), {}).item<long>()].item<int>() : 0;
            }
            actionT en_act = targetNet.MapIndexToAction(en, pl, en_act_idx);
            en.TakeAction(en_act);

            steps++;
            reward -= 0.01f;

            if (steps % 256 == 0) {
                at::Tensor masked_output = q_values.masked_fill(mask == 0, -1e10);
                at::Tensor probs = torch::softmax(masked_output, 1);
                ShowInMap(pl, en, map, probs, "dqn");

                for (const auto& [action, count] : action_count_map) {
                    std::cout << action << ": " << count << " | ";
                }
                std::cout << "p.gold: " << pl.gold << "|en.gold: " << en.gold 
                          << "|pun: " << pl.units.size() << "|eun: " << en.units.size() 
                          << " | Steps: " << steps << std::endl;
            }

            bool envDone = ShouldResetEnvironment(pl, en, map); 
            done = envDone || (steps >= 5000);
            if (envDone) {
                 const int hallCost = 590; 
                 bool plHasPeasant = pl.HasUnit(PEASANT);
                 bool plHasHall = pl.HasStructure(HALL);
                 bool plLost = (!plHasPeasant && !plHasHall) || 
                               (!plHasPeasant && pl.gold < 55) || 
                               (plHasPeasant && pl.gold < hallCost && !plHasHall);

                 bool enLost = (!en.HasUnit(PEASANT) && !en.HasStructure(HALL)); 
             
                 if (plLost) {
                     reward -= 5.0f;
                 } else if (enLost){
                     reward += 5.0f;
                 }
             }

            total_reward += reward;
            State nextState = GetState(pl, en, map);
            torch::Tensor ns_tensor = TensorStruct(nextState, map).GetTensor().clone();

            AddExperience(Transition(s_tensor, act_idx, ns_tensor, reward, done));

            if (steps % 64 == 0 && memory.size() >= 512) {
                OptimizeDQN(map, optimizer); 
                
                torch::NoGradGuard no_grad;
                auto target_params = targetNet.parameters();
                auto policy_params = policyNet.parameters();
                for (size_t k = 0; k < target_params.size(); k++) {
                    target_params[k].copy_(target_params[k] * (1.0 - tau) + policy_params[k] * tau);
                }
            }
        }
        
        epsilon = std::max(0.05f, epsilon - epsilonDecay);
        std::cout << "Episode: " << i << " | Steps: " << steps << " | Reward: " << total_reward << std::endl;
        
        map.Reset();
        pl.Reset(pl.side);
        en.Reset(en.side);

        if ((i % 10 == 0 && i != 0) || i == episodeNumber - 1) {
            SaveMemoryAsBinary(); 
            std::string time_suffix = get_current_time();
            if (pl.side == PLAYER) {
                policyNet.SaveModel("models/player_models_dqn/dqn_policy-" + time_suffix); 
                targetNet.SaveModel("models/enemy_models_dqn/dqn_policy-" + time_suffix);
            } else {
                policyNet.SaveModel("models/enemy_models_dqn/dqn_policy-" + time_suffix); 
                targetNet.SaveModel("models/player_models_dqn/dqn_policy-" + time_suffix);
            }
        }
    }
}

void RlManager::OptimizeDQN(Map &map, torch::optim::AdamW& optimizer) {
    int batch_size = 512;
    if (memory.size() < batch_size) return;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<size_t> dist(0, memory.size() - 1);

    std::vector<torch::Tensor> states, actions, next_states, rewards, dones;

    for (int i = 0; i < batch_size; ++i) {
        const auto& trans = memory[dist(rng)];
        states.push_back(trans.state); // Instant access
        next_states.push_back(trans.nextState);
        actions.push_back(torch::tensor({(int64_t)trans.actionIndex}));
        rewards.push_back(torch::tensor({std::clamp(trans.reward, -1.0f, 1.0f)}));
        dones.push_back(torch::tensor({trans.done ? 0.0f : 1.0f}));
    }

    auto tensor_states = torch::cat(states);
    auto tensor_next_states = torch::cat(next_states);
    auto tensor_actions = torch::cat(actions).unsqueeze(1);
    auto tensor_rewards = torch::cat(rewards).unsqueeze(1);
    auto tensor_dones = torch::cat(dones).unsqueeze(1);

    torch::Tensor q_values = policyNet.Forward(tensor_states).gather(1, tensor_actions);

    torch::Tensor q_next_values;
    {
        torch::NoGradGuard no_grad;
        q_next_values = std::get<0>(targetNet.Forward(tensor_next_states).max(1)).unsqueeze(1);
        q_next_values = tensor_rewards + (gamma * q_next_values * tensor_dones);
    }

    auto loss = torch::nn::functional::smooth_l1_loss(q_values, q_next_values);
    optimizer.zero_grad();
    loss.backward();
    torch::nn::utils::clip_grad_norm_(policyNet.parameters(), 1.0f);
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
    std::ofstream file("memory_string.txt");
    if (!file.is_open()) return;

    for (const auto& trans : memory) {
        // Format: ActionIndex,Reward,Done,InputSize|StateData|NextStateData
        file << trans.actionIndex << "," << trans.reward << "," << (trans.done ? 1 : 0) << ",";
        
        int inputSize = trans.state.size(1);
        file << inputSize << "|";

        float* s_ptr = trans.state.data_ptr<float>();
        for(int i = 0; i < inputSize; ++i) file << s_ptr[i] << (i == inputSize - 1 ? "" : ",");
        
        file << "|";

        float* ns_ptr = trans.nextState.data_ptr<float>();
        for(int i = 0; i < inputSize; ++i) file << ns_ptr[i] << (i == inputSize - 1 ? "" : ",");
        
        file << "\n";
    }
    file.close();
}

void RlManager::LoadMemoryAsString() {
    std::ifstream file("memory_string.txt");
    if (!file.is_open()) return;

    memory.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string segment;
        
        // 1. Metadata
        std::getline(ss, segment, '|');
        std::stringstream meta_ss(segment);
        std::string val;
        std::getline(meta_ss, val, ','); int act = std::stoi(val);
        std::getline(meta_ss, val, ','); float rew = std::stof(val);
        std::getline(meta_ss, val, ','); bool d = std::stoi(val) == 1;
        std::getline(meta_ss, val, ','); int inSize = std::stoi(val);

        // 2. State Tensor Data
        std::vector<float> s_data(inSize);
        std::getline(ss, segment, '|');
        std::stringstream s_ss(segment);
        for(int i = 0; i < inSize; ++i) {
            std::getline(s_ss, val, ',');
            s_data[i] = std::stof(val);
        }

        // 3. Next State Tensor Data
        std::vector<float> ns_data(inSize);
        std::getline(ss, segment, '|');
        std::stringstream ns_ss(segment);
        for(int i = 0; i < inSize; ++i) {
            std::getline(ns_ss, val, ',');
            ns_data[i] = std::stof(val);
        }

        torch::Tensor s = torch::from_blob(s_data.data(), {1, inSize}).clone();
        torch::Tensor ns = torch::from_blob(ns_data.data(), {1, inSize}).clone();
        memory.push_back(Transition(s, act, ns, rew, d));
    }
}

void RlManager::SaveMemoryAsBinary() {
    std::ofstream file("binary.bay", std::ios::binary);
    if (!file.is_open()) return;

    size_t memSize = memory.size();
    file.write(reinterpret_cast<char*>(&memSize), sizeof(memSize));

    for (const auto& trans : memory) {
        // Save metadata
        file.write(reinterpret_cast<const char*>(&trans.actionIndex), sizeof(int));
        file.write(reinterpret_cast<const char*>(&trans.reward), sizeof(float));
        file.write(reinterpret_cast<const char*>(&trans.done), sizeof(bool));

        // Save Tensors (Flatten to float arrays)
        int inputSize = trans.state.size(1);
        file.write(reinterpret_cast<const char*>(&inputSize), sizeof(int));
        
        file.write(reinterpret_cast<const char*>(trans.state.data_ptr<float>()), inputSize * sizeof(float));
        file.write(reinterpret_cast<const char*>(trans.nextState.data_ptr<float>()), inputSize * sizeof(float));
    }
    file.close();
}

void RlManager::LoadMemoryAsBinary() {
    std::ifstream file("binary.bay", std::ios::binary);
    if (!file.is_open()) return;

    size_t memSize;
    file.read(reinterpret_cast<char*>(&memSize), sizeof(memSize));
    memory.clear();

    for (size_t i = 0; i < memSize; ++i) {
        int act; float rew; bool d; int inSize;
        file.read(reinterpret_cast<char*>(&act), sizeof(int));
        file.read(reinterpret_cast<char*>(&rew), sizeof(float));
        file.read(reinterpret_cast<char*>(&d), sizeof(bool));
        file.read(reinterpret_cast<char*>(&inSize), sizeof(int));

        std::vector<float> s_data(inSize), ns_data(inSize);
        file.read(reinterpret_cast<char*>(s_data.data()), inSize * sizeof(float));
        file.read(reinterpret_cast<char*>(ns_data.data()), inSize * sizeof(float));

        // Reconstruct Tensors from raw bytes
        torch::Tensor s = torch::from_blob(s_data.data(), {1, inSize}).clone();
        torch::Tensor ns = torch::from_blob(ns_data.data(), {1, inSize}).clone();

        memory.push_back(Transition(s, act, ns, rew, d));
    }
    std::cout<<"DQN memory loaded"<<std::endl;
    file.close();
}



// OLD WITH STATE BUT ERROR PRONE BECAUSE OF DANGLING POITNERS CHANGED TO JUST PLAIN TENSORS


//void RlManager::SaveMemoryAsBinary() {
//    std::string memory_file_binary = "models/player_dqn_experience/memory_" + get_current_time() + ".bay";
//    std::vector<binary> data_to_save;
//    std::ofstream file;
//    file.open(memory_file_binary, std::ios::binary);
//    data_to_save.reserve(memory.size() * 1024);
//
//    // Accumulate all serialized vectors
//    for (int i = 0; i < memory.size(); i++) {
//        std::vector<binary> data = memory[i].SerializeBinary();
//        data_to_save.insert(data_to_save.end(), data.begin(), data.end());
//    }
//    file.write(reinterpret_cast<char*>(data_to_save.data()), data_to_save.size() * sizeof(binary));
//
//    file.close();
//}
//
//
//// 0-11 first bytes,
//void RlManager::LoadMemoryAsBinary() {
//    std::string memory_file_binary = get_latest_model("models/player_dqn_experience", "memory_", "bay");
//    std::ifstream file(memory_file_binary, std::ios::binary);
//    std::vector<binary> binaryData;
//    int expectedBytes = 0;
//    binary temp;
//    int count = 0;
//
//    if (!file.is_open())
//    {
//        std::cout << "Binary replay file couldn't be opened.";
//        return;
//    }
//
//    while (file.read(reinterpret_cast<char *>(&temp), sizeof(binary)))
//    {
//        if (binaryData.size() == 0)
//        {
//            expectedBytes = std::get<int>(temp);
//            binaryData.resize(expectedBytes);
//            // std::cout<<expectedBytes<<" ";
//            continue;
//        }
//        binaryData[count] = temp;
//
//        if (count == expectedBytes - 1)
//        {
//            Transition trans;
//            trans = trans.DeserializeBinary(binaryData);
//            AddExperience(trans);
//            binaryData.clear();
//            count = 0;
//            continue;
//        }
//        count++;
//    }
//    file.close();
//}
//
//void RlManager::SaveMemoryAsString() {
//    std::string data_to_save = "";
//    std::string memory_file = "models/player_dqn_experience/memory_" + get_current_time() + ".say";
//    for (int i = 0; i < memory.size(); i++)
//    {
//        data_to_save += memory[i].Serialize() + "\n";
//    }
//    std::ofstream file;
//    file.open(memory_file);
//    file << data_to_save;
//    file.close();
//}
//
//
//void RlManager::LoadMemoryAsString() {
//
//    std::string memory_file = get_latest_model("models/player_dqn_experience", "memory_", "say");
//    std::ifstream file(memory_file);
//    if (!file.is_open()) {
//        std::cout << "String replay file couldn't be opened.\n";
//        return;
//    }
//
//    std::string fileContents((std::istreambuf_iterator<char>(file)),
//                             std::istreambuf_iterator<char>());
//
//    file.close();
//    Transition trans;
//    trans = trans.Deserialize(fileContents);
//    AddExperience(trans);
//}
