#ifndef PPO_H
#define PPO_H

#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>
#include <string>
#include "Tensor.h"
#include "../Tools/Macro.h"
#include "../Tools/Enums.h"




class PPO : public torch::nn::Module{
    public:
        PPO();
        void Initialize(Map &map, State &s);
        torch::Tensor Forward(torch::Tensor x);
        actionT MapIndexToAction(Player &pl, Player &en, int actionIndex);
        void SaveModel(std::string model_name);
        void LoadModel(std::string model_name);

    private:
        void QValue();
        void ActionValue();
        float GetActionReward();
    public:
        torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};
        torch::nn::Tanh tanh;


    private:
        const float epsilon = 0.2f;
        const float discount = 0.9f;
        int actionSize = 0;
        int inputSize = 0;


        int mapSize = MAP_SIZE * MAP_SIZE;
        int moveAction = MAP_SIZE * MAP_SIZE * MAX_UNITS;
        int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
        int buildAction =
            attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE;
        int farmAction =
            buildAction + PEASANT_INDEX_IN_UNITS * MAP_SIZE * MAP_SIZE *
                              HALL_INDEX_IN_STRCTS; // town hall size multipled here as well
        int recruitAction =
            farmAction + /*2 idk why 2 here wtf*/ NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size        
};

#endif
