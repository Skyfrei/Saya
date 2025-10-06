#include "ValueNetwork.h"

ValueNetwork::ValueNetwork() {
}

torch::Tensor ValueNetwork::Forward(torch::Tensor x){
    x = tanh(layer1(x));
    x = tanh(layer2(x));
    x = tanh(layer3(x));
    return x;
}
