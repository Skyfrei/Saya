#include "DQN.h"
DQN::DQN(int inputSize, int actionNumber) {
  layer1 = register_module("layer1", torch::nn::Linear(inputSize, 128));
  layer2 = register_module("layer2", torch::nn::Linear(128, 128));
  layer3 = register_module("layer3", torch::nn::Linear(128, actionNumber));
}

torch::nn::Linear DQN::Forward(torch::Tensor x) {
  x = torch::relu(layer1(x));
  x = torch::relu(layer2(x));
  return layer3;
}

void DQN::SelectAction(State state, DQN policy) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist1(0.0, 1.0);


  float epsilonThreshold = endEpsilon + (startEpsilon - endEpsilon) * std::exp(-1. * stepsDone / epsilonDecay);
  stepsDone++;  
  float sample = dist1(rng);

  if (sample > epsilonThreshold) {
    action = policy->forward(state).max(1).indices().view({1, 1});
    return action;
  } 
  else {
    torch::Tensor action = torch::randint(0, actionSpaceSize - 1, {1}).to(torch::kLong).to(device);
    return action;
  }
}

//void DQN::OptimizeModel(){
//    if (memory.size() < BATCH_SIZE) {
//        return;
//    }
//    std::vector<Transition> transitions = sample_memory(BATCH_SIZE);
//
//    std::vector<torch::Tensor> states, actions, rewards, next_states;
//    for (const auto& t : transitions) {
//        states.push_back(t.state);
//        actions.push_back(t.action);
//        rewards.push_back(t.reward);
//        next_states.push_back(t.next_state);
//    }
//
//    torch::Tensor state_batch = torch::cat(states).to(device);
//    torch::Tensor action_batch = torch::cat(actions).to(device);
//    torch::Tensor reward_batch = torch::cat(rewards).to(device);
//
//    std::vector<torch::Tensor> non_final_next_states_vec;
//    std::vector<int64_t> non_final_mask_vec;
//    for (size_t i = 0; i < next_states.size(); ++i) {
//        if (next_states[i].defined()) {
//            non_final_next_states_vec.push_back(next_states[i]);
//            non_final_mask_vec.push_back(1);
//        } else {
//            non_final_mask_vec.push_back(0);
//        }
//    }
//
//    torch::Tensor non_final_next_states = torch::cat(non_final_next_states_vec).to(device);
//    torch::Tensor non_final_mask = torch::tensor(non_final_mask_vec, torch::TensorOptions().device(device).dtype(torch::kBool));
//
//    torch::Tensor state_action_values = policy_net->forward(state_batch).gather(1, action_batch);
//
//    torch::Tensor next_state_values = torch::zeros({BATCH_SIZE}, torch::TensorOptions().device(device));
//    {
//        torch::NoGradGuard no_grad;
//        next_state_values.index_put_({non_final_mask}, target_net->forward(non_final_next_states).max(1).values());
//    }
//
//    torch::Tensor expected_state_action_values = (next_state_values * GAMMA) + reward_batch;
//
//    torch::nn::SmoothL1Loss criterion;
//    torch::Tensor loss = criterion(state_action_values, expected_state_action_values.unsqueeze(1));
//
//    optimizer->zero_grad();
//    loss.backward();
//    torch::nn::utils::clip_grad_value_(policy_net->parameters(), 100);
//    optimizer->step();
//}
