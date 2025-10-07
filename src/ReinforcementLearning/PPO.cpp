#include "PPO.h"
#include "../Race/Structure/TownHall.h"
#include "../Tools/Binary.h"

PPO::PPO(){

}

void PPO::Initialize(Map &map, State &s){
    TensorStruct tensor = TensorStruct(s, map);
    inputSize = tensor.GetTensor().size(1);
    actionSize = recruitAction;
    layer1 = register_module("layer1", torch::nn::Linear(inputSize, 128));
    layer2 = register_module("layer2", torch::nn::Linear(128, 128));
    layer3 = register_module("layer3", torch::nn::Linear(128, actionSize));
    tanh = register_module("tanh", torch::nn::Tanh());

}

torch::Tensor PPO::Forward(torch::Tensor x){
    x = tanh(layer1(x));
    x = tanh(layer2(x));
    x = tanh(layer3(x));
    return x;
}

void PPO::QValue(){
    //return action array
}

void PPO::ActionValue(){
    //returns reward
}

float PPO::GetActionReward(){
    return 0.2f;
}

void PPO::SaveModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(model_name);

}
void PPO::LoadModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::InputArchive archive;
    archive.load_from(model_name);
    this->load(archive);
}


actionT PPO::MapIndexToAction(Player &pl, Player &en, int actionIndex) {
    if (actionIndex < moveAction)
    {
        int col = actionIndex % MAP_SIZE;
        int row = (actionIndex / MAP_SIZE) % MAP_SIZE;
        int unitIndex = (actionIndex / (MAP_SIZE * MAP_SIZE));
        if (unitIndex >= pl.units.size())
            return EmptyAction();
        return MoveAction(pl.units[unitIndex].get(), Vec2(row, col));
    }
    else if (actionIndex < attackAction)
    {
        int offset = actionIndex - moveAction;
        int playerUnit = (offset / (MAX_STRUCTS + MAX_UNITS)) % MAX_UNITS;
        if (playerUnit >= pl.units.size())
            return EmptyAction();
        int targetIndex = offset % (MAX_STRUCTS + MAX_UNITS);
        if (targetIndex <= MAX_STRUCTS - 1)
        {
            if (targetIndex >= en.structures.size())
                return EmptyAction();
            return AttackAction(pl.units[playerUnit].get(),
                                en.structures[targetIndex].get());
        }
        else
        {
            if (targetIndex >= en.units.size())
                return EmptyAction();
            return AttackAction(pl.units[playerUnit].get(),
                                en.units[targetIndex].get());
        }
    }
    else if (actionIndex < buildAction)
    {
        int offset = actionIndex - attackAction;
        int unit = offset / (NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE);
        StructureType struSelect = static_cast<StructureType>(
            (offset / MAP_SIZE * MAP_SIZE) % NR_OF_STRUCTS);
        int mapSelect = offset % (MAP_SIZE * MAP_SIZE);
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        if (unit >= pl.units.size())
        {
            return EmptyAction();
        }
        if (pl.units[unit]->is != PEASANT)
            return EmptyAction();
        return BuildAction(pl.units[unit].get(), struSelect, Vec2(row, col));
    }
    else if (actionIndex < farmAction)
    {
        int offset = actionIndex - buildAction;
        int peasantIndex = offset / (mapSize * HALL_INDEX_IN_STRCTS);
        int hallIndex = (offset / mapSize) % HALL_INDEX_IN_STRCTS;
        int mapSelect = offset % mapSize;
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        if (peasantIndex >= pl.units.size())
            return EmptyAction();
        if (hallIndex >= pl.structures.size())
            return EmptyAction();
        if (pl.units[peasantIndex]->is != PEASANT)
            return EmptyAction();
        if (pl.structures[hallIndex]->is != HALL)
            return EmptyAction();
        return FarmGoldAction(
            pl.units[peasantIndex].get(), Vec2(row, col),
            static_cast<TownHall *>(pl.structures[hallIndex].get()));
    }
    else if (actionIndex < recruitAction)
    {
        int offset = actionIndex - farmAction;
        UnitType unitType = static_cast<UnitType>(offset / BARRACK_INDEX_IN_STRUCTS);
        int barrackIndex = offset % BARRACK_INDEX_IN_STRUCTS;
        if (barrackIndex >= pl.structures.size())
            return EmptyAction();
        if (pl.structures[barrackIndex]->is != BARRACK)
            return EmptyAction();
        return RecruitAction(unitType, pl.structures[barrackIndex].get());
    }
    return EmptyAction();
}

