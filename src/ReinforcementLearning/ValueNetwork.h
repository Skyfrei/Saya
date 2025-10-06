#ifndef VALUENETWORK_H
#define VALUENETWORK_H

#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>
#include <string>
#include "Tensor.h"
#include "../Tools/Macro.h"
#include "../Tools/Enums.h"
#include "DQN.h"
#include "PPO.h"

class ValueNetwork : public torch::nn::Module{
    public:
        ValueNetwork();
        torch::Tensor Forward(torch::Tensor x);


        template<typename Policy>
        void Initialize(Policy& policy) {
            auto l1_in = policy.layer1->options.in_features();
            auto l1_out = policy.layer1->options.out_features();
            layer1 = register_module("layer1", torch::nn::Linear(l1_in, l1_out));
        
            auto l2_in = policy.layer2->options.in_features();
            auto l2_out = policy.layer2->options.out_features();
            layer2 = register_module("layer2", torch::nn::Linear(l2_in, l2_out));
            
            layer3 = register_module("layer3", torch::nn::Linear(128, 1));
            tanh = register_module("tanh", torch::nn::Tanh());
        }

    
    private:
        torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};
        torch::nn::Tanh tanh;
};

#endif
