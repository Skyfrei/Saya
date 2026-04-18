#include "DQN.h"
#include "../Race/Structure/TownHall.h"
#include "../Tools/Binary.h"
#include "../Tools/Macro.h"
#include <algorithm>
#include <cmath>
#include <map>


DQN::DQN() :gen(std::random_device{}()), uniform_dist(0, recruitAction - 1)  {
}
void DQN::Initialize(Map &map, State &s) {
    TensorStruct tensor = TensorStruct(s, map);
    inputSize = tensor.GetTensor().size(1);
    actionSize = recruitAction;
    layer1 = register_module("layer1", torch::nn::Linear(inputSize, 128));
    layer2 = register_module("layer2", torch::nn::Linear(128, 128));
    layer3 = register_module("layer3", torch::nn::Linear(128, actionSize));
    tanh = register_module("tanh", torch::nn::Tanh());

}

torch::Tensor DQN::Forward(torch::Tensor x) {
    x = tanh(layer1(x));
    x = tanh(layer2(x));
    x = layer3(x);
    return x;
}

std::tuple<actionT, int> DQN::SelectAction(Player &pl, Player &en, Map &map,
                                           State &s, float epsilon) {
    torch::NoGradGuard no_grad;
    float random_number = torch::rand({1}).item<float>();

    TensorStruct dqn_input = TensorStruct(s, map);
    at::Tensor output = Forward(dqn_input.GetTensor());

    if (random_number > epsilon)
    {
        at::Tensor actionIndex = std::get<1>(output.max(1));
        actionT action = MapIndexToAction(pl, en, actionIndex.item<int>());
        return {action, actionIndex.item<int>()};
    }
    else
    {
        int random_action = GetRandomOutputIndex(); 
        actionT result = MapIndexToAction(pl, en, random_action);
        return {result, random_action};
    }
}

actionT DQN::MapIndexToAction(Player &pl, Player &en, int actionIndex) {
    if (actionIndex < moveAction)
    {
        int col = actionIndex % MAP_SIZE;
        int row = (actionIndex / MAP_SIZE) % MAP_SIZE;
        int unitIndex = (actionIndex / (MAP_SIZE * MAP_SIZE));

        if (unitIndex >= pl.units.size())
            return EmptyAction();

        Unit* actor = pl.units[unitIndex].get();
        if (!actor)
            return EmptyAction();

        if (actor->coordinate == Vec2(row, col))
            return EmptyAction();

        return MoveAction(actor, Vec2(row, col));
    }
    else if (actionIndex < attackAction)
    {
        int offset = actionIndex - moveAction;
        int playerUnit = (offset / (MAX_STRUCTS + MAX_UNITS)) % MAX_UNITS;
        if (playerUnit >= pl.units.size())
            return EmptyAction();
        Unit* actor = pl.units[playerUnit].get();
        if (!actor)
            return EmptyAction();
        if (dynamic_cast<Peasant*>(actor))
            return EmptyAction();

        int targetIndex = offset % (MAX_STRUCTS + MAX_UNITS);
        if (targetIndex <= MAX_STRUCTS - 1)
        {
            if (targetIndex >= en.structures.size())
                return EmptyAction();
            return AttackAction(actor, en.structures[targetIndex].get());
        }
        else
        {
            if (targetIndex >= en.units.size())
                return EmptyAction();
            return AttackAction(actor, en.units[targetIndex].get());
        }
    }
    else if (actionIndex < buildAction)
    {
        int offset = actionIndex - attackAction;
        int unit = offset / (NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE);

        StructureType struSelect = static_cast<StructureType>(
            (offset / (MAP_SIZE * MAP_SIZE)) % NR_OF_STRUCTS);
        int mapSelect = offset % (MAP_SIZE * MAP_SIZE);
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        if (unit >= pl.units.size())
        {
            return EmptyAction();
        }
        if (pl.units[unit]->is != PEASANT)
            return EmptyAction();

        int diff = pl.food.y - pl.food.x;
        if (diff >= 10 && struSelect == FARM){
            return EmptyAction();
        }
        if (pl.food.y >= MAX_STRUCTS && struSelect == FARM){
            return EmptyAction();
        }
        int hall_size = 0;
        int barrack_size = 0;
        for (auto& c : pl.structures){
            if (c->is == HALL)
                hall_size++;
            else if (c->is == BARRACK)
                barrack_size++;
        }
        if (struSelect == BARRACK && barrack_size >= BARRACK_INDEX_IN_STRUCTS)
            return EmptyAction();
        else if (struSelect == HALL && hall_size >= HALL_INDEX_IN_STRCTS)
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
        if (pl.units[peasantIndex]->is != PEASANT)
            return EmptyAction();
        if (hallIndex >= pl.structures.size())
            return EmptyAction();
        if (pl.structures[hallIndex]->is != HALL)
            return EmptyAction();
        if (pl.gold > 590)
            return EmptyAction();

        return FarmGoldAction(
            pl.units[peasantIndex].get(),
            Vec2(row, col),
            static_cast<TownHall *>(pl.structures[hallIndex].get()));
    }
    else if (actionIndex < recruitAction)
    {
        int offset = actionIndex - farmAction;
        
        int unitTypeInt = offset / MAX_STRUCTS;
        int structureIndex = offset % MAX_STRUCTS;

        if (unitTypeInt >= NR_OF_UNITS){
            return EmptyAction();
        }
        if (structureIndex >= pl.structures.size()){
            return EmptyAction();
        }

        UnitType unitType = static_cast<UnitType>(unitTypeInt);
        Structure* stru = pl.structures[structureIndex].get();

        if (unitType == PEASANT) {
            if (pl.gold < 55) return EmptyAction();
            if (stru->is != HALL) return EmptyAction();
            int p_count = 0;
            for (auto& c : pl.units){
                if (c.get()->is == PEASANT)
                    p_count++;
            }
            if (p_count >= PEASANT_INDEX_IN_UNITS)
                return EmptyAction();
        } 
        else if (unitType == FOOTMAN) {
            if (pl.gold < 75) return EmptyAction();
            if (stru->is != BARRACK) return EmptyAction();
        }

        return RecruitAction(unitType, stru);
    }
    return EmptyAction();
}

actionT DQN::SelectAction(State &state, Map &map, float epsilon) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist1(0.0, 1.0);
    float random_number = dist1(rng);

    torch::NoGradGuard no_grad;
    if (random_number > epsilon)
    {
        TensorStruct dqn_input = TensorStruct(state, map);
        at::Tensor action =
            std::get<1>(Forward(dqn_input.GetTensor()).max(1)).view({1, 1});
        actionT result = MapIndexToAction(state, action.item<int>());
        return result;
    }
    else
    {
        TensorStruct dqn_input = TensorStruct(state, map);
        at::Tensor output = Forward(dqn_input.GetTensor()).view({1, 1});
        int num_actions = output.size(1);
        std::uniform_int_distribution<std::mt19937::result_type> dist(
            0, num_actions - 1);
        int random_action = dist(rng);
        at::Tensor action = torch::tensor({{random_action}}, torch::kLong);
        actionT result = MapIndexToAction(state, action.item<int>());
        return result;
    }
}

actionT DQN::MapIndexToAction(State &state, int actionIndex) {
    if (actionIndex < moveAction)
    {
        int col = actionIndex % MAP_SIZE;
        int row = (actionIndex / MAP_SIZE) % MAP_SIZE;
        int unitIndex = (actionIndex / (MAP_SIZE * MAP_SIZE));
        if (unitIndex >= state.playerUnits.size())
            return EmptyAction();
        return MoveAction(state.playerUnits[unitIndex], Vec2(row, col));
    }
    else if (actionIndex < attackAction)
    {
        int offset = actionIndex - moveAction;
        int playerUnit = (offset / (MAX_STRUCTS + MAX_UNITS)) % MAX_UNITS;
        if (playerUnit >= state.playerUnits.size())
            return EmptyAction();
        int targetIndex = offset % (MAX_STRUCTS + MAX_UNITS);
        if (targetIndex < MAX_STRUCTS - 1)
        {
            if (targetIndex >= state.enemyStructs.size())
                return EmptyAction();
            return AttackAction(state.playerUnits[playerUnit],
                                state.enemyStructs[targetIndex]);
        }
        else
        {
            if (targetIndex >= state.enemyUnits.size())
                return EmptyAction();
            return AttackAction(state.playerUnits[playerUnit],
                                state.enemyUnits[targetIndex]);
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
        if (unit >= state.playerUnits.size())
        {
            return EmptyAction();
        }
        if (state.playerUnits[unit]->is != PEASANT)
            return EmptyAction();
        //std::cout<<"Unit: "<<unit<<std::endl;
        //std::cout<<"Unit size: "<<state.playerUnits.size()<<std::endl;

        return BuildAction(state.playerUnits[unit], struSelect, Vec2(row, col));
    }
    else if (actionIndex < farmAction)
    {
        int offset = actionIndex - buildAction;
        int peasantIndex = offset / (mapSize * HALL_INDEX_IN_STRCTS);
        int hallIndex = (offset / mapSize) % HALL_INDEX_IN_STRCTS;
        int mapSelect = offset % mapSize;
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        if (peasantIndex >= state.playerUnits.size())
            return EmptyAction();
        if (hallIndex >= state.playerStructs.size())
            return EmptyAction();
        if (state.playerUnits[peasantIndex]->is != PEASANT)
            return EmptyAction();
        if (state.playerStructs[hallIndex]->is != HALL)
            return EmptyAction();
        return FarmGoldAction(
            state.playerUnits[peasantIndex], Vec2(row, col),
            static_cast<TownHall *>(state.playerStructs[hallIndex]));
    }
    else if (actionIndex < recruitAction)
    {
        int offset = actionIndex - farmAction;
        UnitType unitType = static_cast<UnitType>(offset / BARRACK_INDEX_IN_STRUCTS);
        int barrackIndex = offset % BARRACK_INDEX_IN_STRUCTS;
        if (barrackIndex >= state.playerStructs.size())
            return EmptyAction();
        if (state.playerStructs[barrackIndex]->is != BARRACK)
            return EmptyAction();
        return RecruitAction(unitType, state.playerStructs[barrackIndex]);
    }
    return EmptyAction();
}

void DQN::PrintWeight() {
    std::cout << this->layer1->weight[0][0] << std::endl;
}

void DQN::SaveModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(model_name);
}

void DQN::LoadModel(std::string model_name){
    model_name += ".pt";
    torch::serialize::InputArchive archive;
    archive.load_from(model_name);
    this->load(archive);
    this->eval();
}



int DQN::GetRandomOutputIndex() {
    return uniform_dist(gen);
}

// actionT DQN::SelectAction(State &state, Map &map, float epsilon) {
//     std::random_device dev;
//     std::mt19937 rng(dev());
//
//     std::uniform_real_distribution<float> dist1(0.0, 1.0);
//     std::uniform_int_distribution<std::mt19937::result_type> dist2(0,
//     NR_OF_ACTIONS - 1); std::uniform_int_distribution<std::mt19937::result_type>
//     pun(
//         0, state.playerUnits.size() - 1);
//     std::uniform_int_distribution<std::mt19937::result_type> pstru(
//         0, state.playerStructs.size() - 1);
//     std::uniform_int_distribution<std::mt19937::result_type> eun(
//         0, state.playerUnits.size() - 1);
//     std::uniform_int_distribution<std::mt19937::result_type> estru(
//         0, state.enemyStructs.size() - 1);
//     std::uniform_int_distribution<std::mt19937::result_type> mapx(0, MAP_SIZE -
//     1); std::uniform_int_distribution<std::mt19937::result_type> mapy(0, MAP_SIZE
//     - 1);
//
//     float random_number = dist1(rng);
//     int cx = mapx(rng);
//     int cy = mapy(rng);
//     int pUnit = pun(rng);
//     int eUnit = eun(rng);
//     int pStru = pstru(rng);
//     int eStru = estru(rng);
//
//     if (random_number > epsilon)
//     {
//         TensorStruct dqn_input = TensorStruct(state, map);
//         at::Tensor action =
//             std::get<1>(Forward(dqn_input.GetTensor()).max(1)).view({1, 1});
//         actionT result = MapIndexToAction(state, action.item<int>());
//         return result;
//     }
//     else
//     {
//         ActionType action_index = static_cast<ActionType>(dist2(rng));
//         switch (action_index)
//         {
//         case MOVE: {
//             MoveAction action = MoveAction(state.playerUnits[pUnit], Vec2(cx,
//             cy)); return action;
//         }
//
//         case ATTACK: {
//             std::uniform_int_distribution<std::mt19937::result_type> struOrEn(0,
//             1); int ran = struOrEn(rng); if (ran == 0)
//             {
//                 AttackAction action =
//                     AttackAction(state.playerUnits[pUnit],
//                     state.enemyUnits[eUnit]);
//                 return action;
//             }
//             else
//             {
//                 AttackAction action =
//                     AttackAction(state.playerUnits[pUnit],
//                     state.enemyStructs[eStru]);
//                 return action;
//             }
//         }
//
//         case BUILD: {
//             std::uniform_int_distribution<std::mt19937::result_type> struType1(
//                 0, NR_OF_STRUCTS - 1);
//             StructureType struType = static_cast<StructureType>(struType1(rng));
//             std::vector<Unit *> peasants;
//             for (auto &p : state.playerUnits)
//             {
//                 if (p->is == PEASANT)
//                     peasants.push_back(p);
//             }
//             if (peasants.size() <= 0)
//                 return EmptyAction();
//             std::uniform_int_distribution<std::mt19937::result_type> peasantX(
//                 0, peasants.size() - 1);
//             BuildAction action =
//                 BuildAction(peasants[peasantX(rng)], struType, Vec2(cx, cy));
//             return action;
//         }
//
//         case FARMGOLD: {
//             std::vector<Unit *> peasants;
//             std::vector<Structure *> halls;
//             for (auto &p : state.playerUnits)
//             {
//                 if (p->is == PEASANT)
//                     peasants.push_back(p);
//             }
//             for (auto &s : state.playerStructs)
//             {
//                 if (s->is == HALL)
//                     halls.push_back(s);
//             }
//
//             if (peasants.size() <= 0 || halls.size() <= 0)
//                 return EmptyAction();
//             std::uniform_int_distribution<std::mt19937::result_type> peasantX(
//                 0, peasants.size() - 1);
//             std::uniform_int_distribution<std::mt19937::result_type> struX(
//                 0, halls.size() - 1);
//             pUnit = peasantX(rng);
//             pStru = struX(rng);
//
//             FarmGoldAction action = FarmGoldAction(peasants[pUnit], Vec2(cx, cy),
//                                                    static_cast<TownHall
//                                                    *>(halls[pStru]));
//             return action;
//         }
//
//         case RECRUIT: {
//             std::uniform_int_distribution<std::mt19937::result_type> unTypeRng(
//                 0, NR_OF_UNITS - 1);
//             std::vector<Structure *> barracks;
//             for (auto &p : state.playerStructs)
//             {
//                 if (p->is == BARRACK)
//                     barracks.push_back(p);
//             }
//             if (barracks.size() <= 0)
//                 return EmptyAction();
//             std::uniform_int_distribution<std::mt19937::result_type> struType(
//                 0, barracks.size() - 1);
//             UnitType unType = static_cast<UnitType>(unTypeRng(rng));
//             int barrackIndex = struType(rng);
//             RecruitAction action = RecruitAction(unType, barracks[barrackIndex]);
//             return action;
//         }
//         case EMPTY: {
//             return EmptyAction();
//         }
//         }
//     }
// }
