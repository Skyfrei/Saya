#ifndef PPO_H
#define PPO_H

#include "../Tools/Macro.h"
#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>
#include <string>


class PPO : public torch::nn::Module{
    public:
        PPO();
        


    private:
        void QValue();
        void ActionValue();

    private:
        const std::string model_name = "ppo_model.pt";
        
};
#endif
