#ifndef TENSOR_H
#define TENSOR_H

#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>

#include "../Race/Unit/Unit.h"
#include "../State/Player.h"
#include "../Tools/Vec2.h"
#include "Transition.h"
#include "../Tools/Macro.h"
#include "../Tools/Enums.h"

struct TensorStruct
{
    TensorStruct(State &state, Map &map);
    torch::Tensor GetMapTensor(Map &map);
    torch::Tensor GetVec(Vec2 food);
    torch::Tensor GetUnitsTensor(std::vector<Unit *> &units);
    torch::Tensor GetStructuresTensor(std::vector<Structure *> &structures);
    torch::Tensor GetTensor();
    const int unitVar = 8;
    const int strucVar = 4;

    torch::Tensor currentMap;
    torch::Tensor playerGold;
    torch::Tensor playerFood;
    torch::Tensor playerUnits;
    torch::Tensor playerStructs;
    torch::Tensor enemyGold;
    torch::Tensor enemyFood;
    torch::Tensor enemyUnits;
    torch::Tensor enemyStructs;
};

#endif
