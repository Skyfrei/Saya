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
    int action_idx = probs.argmax(1).item<int>();
    actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);

    ShowInMap(pl, en, map, probs);
    //int k = std::min(5, (int)probs.size(1));
    //auto topk_res = probs.topk(k, 1);
    //auto top_probs = std::get<0>(topk_res);  
    //auto top_indices = std::get<1>(topk_res);

    //int sample_idx = torch::multinomial(top_probs, 1).item<int>();
    //int action_idx = top_indices[0][sample_idx].item<int>();
    //actionT action = ppoPolicy.MapIndexToAction(pl, en, action_idx);


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
    int action_idx = probs.argmax(1).item<int>();
    actionT action = enemyPPO.MapIndexToAction(en, pl, action_idx);

    //int k = std::min(10, (int)probs.size(1));
    //auto topk_res = probs.topk(k, 1);
    //auto top_probs = std::get<0>(topk_res);  
    //auto top_indices = std::get<1>(topk_res);

    //int sample_idx = torch::multinomial(top_probs, 1).item<int>();
    //int action_idx = top_indices[0][sample_idx].item<int>();
    //actionT action = enemyPPO.MapIndexToAction(en, pl, action_idx);
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
    enemyPPO.LoadModel(get_random_model("models/"));

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

bool RlManager::ShouldResetEnvironment(Player &pl, Player &en, Map &map) {
    const int hallCost = 590;

    auto Reset = [&](Player& p){
        bool hasPeasant = p.HasUnit(PEASANT);
        bool hasHall = p.HasStructure(HALL);

        if (!hasPeasant && !hasHall) return true;

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
            mask.narrow(1, unitMoveStart, MAP_SIZE * MAP_SIZE).fill_(1.0f); // Enable all tiles

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
                    mask[0][unitAttackStart + t] = 1.0f; // Enable if target exists
                }
            }
        }
    }

    // Build
    if (pl.gold >= 100) {
        for (int u = 0; u < PEASANT_INDEX_IN_UNITS; u++) {
            if (u < pl.units.size() && pl.units[u]->is == PEASANT && pl.units[u]->GetActionQueueSize() == 0) {
                int startIdx = attackEnd + (u * buildStride);
                mask.narrow(1, startIdx, buildStride).fill_(1.0f); // Enable building
            }
        }
    }

    // Farm
    for (int u = 0; u < PEASANT_INDEX_IN_UNITS; u++) {
        if (u < pl.units.size() && pl.units[u]->is == PEASANT && pl.units[u]->GetActionQueueSize() == 0) {
            for (int h = 0; h < HALL_INDEX_IN_STRCTS; h++) {
                if (h < pl.structures.size() && pl.structures[h]->is == HALL) {
                    int startIdx = buildEnd + (u * MAP_SIZE * MAP_SIZE * HALL_INDEX_IN_STRCTS) + (h * MAP_SIZE * MAP_SIZE);
                    // Only enable tiles with resources
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
        if (structureIndex < pl.structures.size()) {
            Structure* stru = pl.structures[structureIndex].get();
            UnitType unitType = static_cast<UnitType>(unitTypeInt);
            if (unitType == PEASANT && stru->is == HALL && pl.gold >= 55) mask[0][recruitStart + offset] = 1.0f;
            else if (unitType == FOOTMAN && stru->is == BARRACK && pl.gold >= 75) mask[0][recruitStart + offset] = 1.0f;
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

    ppoPolicy.LoadModel("models/ppo_policy-02-27_14-19");
    double start_lr = 8e-4; // A bit "bigger" to start
    double end_lr = 1e-4;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, ppoPolicy.layer3->options.out_features() - 1);

    bool done = false;
    
    for (int i = 0; i < episodeNumber; i++){
        if (i % 10 == 0 && i != 0){
            ppoPolicy.SaveModel("models/ppo_policy-" + get_current_time());
            enemyPPO.LoadModel(get_random_model("models/"));
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
                // Player move ends

                // Enemy move
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

                ShowInMap(pl, en, map, output_soft);
                bool envDone = ShouldResetEnvironment(pl, en, map);
                bool timeDone = (fullSteps >= 5000);
                done = envDone || timeDone;

               if (envDone) {
                    const int hallCost = 580; 
                    bool plLost = (!pl.HasUnit(PEASANT) && !pl.HasStructure(HALL));
                
                    bool enLost = (!en.HasUnit(PEASANT) && !en.HasStructure(HALL)); 
                
                    if (plLost) {
                        reward -= 500.0f;
                    } 
                    else if (enLost) {
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
                gae = delta + gamma * (1 - currDone) * gae;
                advantages_buffer[t] = gae.clone().detach();
                returns_buffer[t] = (gae + value).clone().detach(); 
            }

            std::cout << "Episode: " << i <<" | Fullsteps: "<< fullSteps << " | Total Reward: " << episode_reward << std::endl;

            for (int k = 0; k < 5; k++){
                policy_optimizer.zero_grad();
                value_optimizer.zero_grad();
                
                at::Tensor total_policy_loss = torch::tensor(0.0f, torch::kFloat32);
                at::Tensor total_value_loss = torch::tensor(0.0f, torch::kFloat32);
                
                for (int t = 0; t < trajectory_buffer.size(); t++) {
                    TensorStruct& input = std::get<0>(trajectory_buffer[t]);                   
                    int action = std::get<1>(trajectory_buffer[t]);
                    at::Tensor old_prob = std::get<4>(trajectory_buffer[t]);
                    at::Tensor old_mask = std::get<6>(trajectory_buffer[t]);

                    
                    at::Tensor advantage = advantages_buffer[t];
                    at::Tensor returns = returns_buffer[t];

                    at::Tensor new_tensor = ppoPolicy.Forward(input.GetTensor());
                    /** applying mask here **/
                    at::Tensor masked_new_tensor = new_tensor.masked_fill(old_mask == 0, -1e10);
                    at::Tensor new_probabs = torch::softmax(masked_new_tensor, -1);
                    /** end of mask here **/
                    at::Tensor new_prob = new_probabs[0][action];

                    at::Tensor ratio = new_prob / old_prob.detach(); 
                    at::Tensor clipped_ratio = torch::clamp(ratio, 1.0f - ppoEpsilon, 1.0f + ppoEpsilon);

                    at::Tensor policy_loss_1 = ratio * advantage.detach();
                    at::Tensor policy_loss_2 = clipped_ratio * advantage.detach();
                    at::Tensor policy_loss = -torch::mean(torch::min(policy_loss_1, policy_loss_2));
                
                    at::Tensor current_value = ppoValue.Forward(input.GetTensor());
                    at::Tensor value_loss = torch::mse_loss(current_value, returns);

                    /** entropy here to make the model explore **/
                    at::Tensor entropy = -(new_probabs * torch::log(new_probabs + 1e-10)).sum(-1).mean();
                    float start_entropy = 0.20f;
                    float end_entropy = 0.05f;
                    float progress = static_cast<float>(i) / episodeNumber; 
                    float entropy_coeff = start_entropy - progress * (start_entropy - end_entropy);
                    total_policy_loss += (policy_loss - entropy_coeff * entropy);
                    /** end **/
                    total_value_loss += value_loss;
                }
                at::Tensor mean_policy_loss = total_policy_loss / forwardSteps;
                at::Tensor mean_value_loss = total_value_loss / forwardSteps;
                at::Tensor total_loss = mean_policy_loss +  mean_value_loss;
                total_loss.backward();
                policy_optimizer.step();
                value_optimizer.step();
                std::cout << "  Policy Loss: " << mean_policy_loss.item<float>() 
                          << " | Value Loss: " << mean_value_loss.item<float>() 
                          << std::endl;
                }
            if (done) {
                map.Reset();
                pl.Reset(PLAYER);
                en.Reset(ENEMY);
            }
        }
    }
    ppoPolicy.SaveModel("ppo_policy");
}

void RlManager::ShowInMap(Player& pl, Player& en, Map& m, at::Tensor& tensor){
    std::string dqn_string = "yes";
    std::string ppo_string = "";

    int k = std::min(8, (int)tensor.size(1));
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
            ppo_string += "Attack " + std::to_string(act.object->coordinate.x) + ", "+ std::to_string(act.object->coordinate.y); 
            
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

    for (int i = 0; i < episodeNumber; i++)
    {
        State currState = GetState(pl, en, map);
        for (int j = 0; j < 1000; j++)
        {

            auto selectedAction =
                policyNet.SelectAction(pl, en, map, currState, epsilon);
            float reward = pl.TakeAction(std::get<0>(selectedAction));
            State nextState = GetState(pl, en, map);
            if (ShouldResetEnvironment(pl, en, map))
                break;
            Transition trans(currState, std::get<0>(selectedAction), nextState,
                             reward, std::get<1>(selectedAction));
            AddExperience(trans);
            selectedAction = policyNet.SelectAction(pl, en, map, nextState, epsilon);
            reward = en.TakeAction(std::get<0>(selectedAction));
            State nextNextState = GetState(pl, en, map);
            if (ShouldResetEnvironment(en, pl, map)) // change order of pl, en to en, pl becasue en takes action here
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
