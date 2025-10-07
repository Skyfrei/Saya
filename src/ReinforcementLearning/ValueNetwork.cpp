#include "ValueNetwork.h"

ValueNetwork::ValueNetwork() {
}

torch::Tensor ValueNetwork::Forward(torch::Tensor x){
    x = tanh(layer1(x));
    x = tanh(layer2(x));
    x = tanh(layer3(x));
    return x;
}

void ValueNetwork::SaveModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(model_name);

}
void ValueNetwork::LoadModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::InputArchive archive;
    archive.load_from(model_name);
    this->load(archive);
}
 
