#ifndef TENSOR_H
#define TENSOR_H

#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>
#include <torch/torch.h>

#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Peasant.h"
#include "../State/Player.h"
#include "../Tools/Vec2.h"
#include "Transition.h"
#include "../Tools/Macro.h"
#include "../Tools/Enums.h"

class TensorStruct {
public:
    TensorStruct(State &state, Map &map);
    torch::Tensor GetTensor();

private:
    torch::Tensor finalTensor; // Holds the perfectly constructed tensor
    const int unitVar = 8;
    const int strucVar = 4;

};
#endif
