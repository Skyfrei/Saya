#include "DQN.h"
#include <cmath>
#include <fstream>
#include <map>
#include <random>
#include <algorithm>
#include "../Tools/Macro.h"
#include "../Tools/Binary.h"
#include "../Tools/Enums.h"
#include "../Race/Structure/TownHall.h"

int mapSize = MAP_SIZE * MAP_SIZE;
int moveAction = MAP_SIZE * MAP_SIZE * MAX_UNITS;
int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
int buildAction = attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE;
int farmAction =
    buildAction + PEASANT_INDEX_IN_UNITS * mapSize * HALL_INDEX_IN_STRCTS;   // town hall size multipled here as well
int recruitAction = farmAction + 2 * NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size



DQN::DQN(){}
void DQN::Initialize(Player& pl, Player& en, Map& map) {
    State s = GetState(pl, en, map);
    TensorStruct tensor = TensorStruct(s, map);
    inputSize = tensor.GetTensor().size(1);
    actionSize = recruitAction;
    layer1 = register_module("layer1", torch::nn::Linear(inputSize, 128));
    layer2 = register_module("layer2", torch::nn::Linear(128, 128));
    layer3 = register_module("layer3", torch::nn::Linear(128, actionSize));
}

torch::Tensor DQN::Forward(torch::Tensor x) {
    x = torch::relu(layer1->forward(x));
    x = torch::relu(layer2->forward(x));
    x = layer3->forward(x);
    return x;
}

void DQN::AddExperience(Transition trans) {
    if (memory.size() >= memory_size)
    {
        memory.pop_front();
    }
    memory.push_back(trans);
}

void DQN::SaveMemory() {
    std::string data_to_save = "";
    for (int i = 0; i < memory.size(); i++)
    {
        data_to_save += memory[i].Serialize() + "\n";
    }
    std::ofstream file;
    file.open(memory_file);
    file << data_to_save;
    file.close();
}

void DQN::LoadMemory() {
    std::ifstream file(memory_file);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open())
    {
        std::cout << "String replay file couldn't be opened.";
        return;
    }
    while (std::getline(file, line))
    {
        Transition trans;
        trans = trans.Deserialize(line);
        // trans = trans.Deserialize(line);
        // AddExperience(trans);
    }
    file.close();
}

void DQN::SaveMemoryAsBinary() {
    std::vector<binary> data_to_save;
    std::ofstream file;
    file.open(memory_file_binary, std::ios::binary);

    for (int i = 0; i < memory.size(); i++)
    {
        std::vector<binary> data = memory[i].SerializeBinary();
        file.write(reinterpret_cast<char *>(data.data()), data.size() * sizeof(binary));
    }

    file.close();
}
// 0-11 first bytes,
void DQN::LoadMemoryAsBinary() {
    std::ifstream file(memory_file_binary, std::ios::binary);
    std::vector<binary> binaryData;
    int expectedBytes = 0;
    binary temp;
    int count = 0;

    if (!file.is_open())
    {
        std::cout << "Binary replay file couldn't be opened.";
        return;
    }

    while (file.read(reinterpret_cast<char *>(&temp), sizeof(binary)))
    {
        if (binaryData.size() == 0)
        {
            expectedBytes = std::get<int>(temp);
            binaryData.resize(expectedBytes);
            // std::cout<<expectedBytes<<" ";
            continue;
        }
        binaryData[count] = temp;

        if (count == expectedBytes - 1)
        {
            Transition trans;
            trans = trans.DeserializeBinary(binaryData);
            AddExperience(trans);
            binaryData.clear();
            count = 0;
            continue;
        }
        count++;
    }
    file.close();
}

void DQN::Train(Player& pl, Player& en, Map& map) {
    float loss = 0.0f;
    float discount = 0.7f;
    int batch_size = 100;
    torch::optim::AdamW optimizer(this->parameters(), 0.01);

    for (int i = 0; i < epochNumber; i++)
    {
        for (int j = 0; j < 1000; j++){
            std::deque<Transition> samples;
            std::sample(memory.begin(), memory.end(),
                        std::back_inserter(samples), batch_size,
                        std::mt19937 {std::random_device{}()});

            State state = GetState(pl, en, map);
            actionT action = SelectAction(pl, en, map);
            float reward = pl.TakeAction(action);  
            if (reward > 0.0f)
                std::cout<<reward;
            State next_state = GetState(pl, en, map);
            Transition trans(state, action, next_state);
            AddExperience(trans);
            // Sample random from experience
            // 
            //with torch.no_grad():
            auto criterion = torch::nn::SmoothL1Loss();
            //auto loss = criterion();
            optimizer.zero_grad();
            //loss.backward();
            torch::nn::utils::clip_grad_value_(this->parameters(), 100);
            optimizer.step();
            
            
            if (epsilon - epsilonDecay > 0)
                epsilon -= epsilonDecay;
        }
    }
}

actionT DQN::SelectAction(Player& pl, Player& en, Map& map) {
    std::random_device dev;
    std::mt19937 rng(dev());
    
    std::uniform_real_distribution<float> dist1(0.0, 1.0);
    std::uniform_int_distribution<std::mt19937::result_type> dist2(0, NR_OF_ACTIONS - 1);
    std::uniform_int_distribution<std::mt19937::result_type> pun(0, pl.units.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> pstru(0, pl.structures.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> eun(0, en.units.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> estru(0, en.structures.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> mapx(0, MAP_SIZE - 1);
    std::uniform_int_distribution<std::mt19937::result_type> mapy(0, MAP_SIZE - 1);

    float random_number = dist1(rng);
    int cx = mapx(rng);
    int cy = mapy(rng);
    int pUnit = pun(rng);
    int eUnit = eun(rng);
    int pStru = pstru(rng);
    int eStru = estru(rng);
    std::cout<<epsilon;

    if (random_number > epsilon){
        State s = GetState(pl, en, map);
        TensorStruct dqn_input = TensorStruct(s, map);
        at::Tensor action = std::get<1>(Forward(dqn_input.GetTensor()).max(1)).view({1, 1});
        actionT result = MapIndexToAction(pl, en, action.item<int>());
        std::cout<<"k"<<std::endl;
        return result;
    }
    else{
        ActionType action_index = static_cast<ActionType>(dist2(rng));
        switch(action_index){
            case MOVE:{
                MoveAction action = MoveAction(pl.units[pUnit].get(), Vec2(cx, cy));
                return action;
            }

            case ATTACK:{
                std::uniform_int_distribution<std::mt19937::result_type> struOrEn(0, 1);
                int ran = struOrEn(rng);
                if (ran == 0){
                    AttackAction action = AttackAction(pl.units[pUnit].get(), en.units[eUnit].get());
                    return action;
                }
                else{
                    AttackAction action = AttackAction(pl.units[pUnit].get(), en.structures[eStru].get());
                    return action;
                }
            }

            case BUILD:{
                std::uniform_int_distribution<std::mt19937::result_type> struType1(0, NR_OF_STRUCTS - 1);
                StructureType struType = static_cast<StructureType>(struType1(rng));
                std::vector<Unit*> peasants;
                for (auto& p : pl.units){
                    if (p->is == PEASANT)
                        peasants.push_back(p.get());
                }
                if (peasants.size() <= 0)
                    return EmptyAction();
                std::uniform_int_distribution<std::mt19937::result_type> peasantX(0, peasants.size() - 1);
                BuildAction action = BuildAction(peasants[peasantX(rng)], struType, Vec2(cx, cy));
                return action;
            }
            
            case FARMGOLD:{
                std::vector<Unit*> peasants;
                std::vector<Structure*> halls;
                for (auto& p : pl.units){
                    if (p->is == PEASANT)
                        peasants.push_back(p.get());
                }
                for (auto& s : pl.structures){
                    if (s->is == HALL)
                        halls.push_back(s.get());
                }

                if (peasants.size() <= 0 || halls.size() <= 0)
                    return EmptyAction();
                std::uniform_int_distribution<std::mt19937::result_type> peasantX(0, peasants.size() - 1);
                std::uniform_int_distribution<std::mt19937::result_type> struX(0, halls.size() - 1);
                pUnit = peasantX(rng);
                pStru = struX(rng);

                FarmGoldAction action = FarmGoldAction(peasants[pUnit], Vec2(cx, cy), static_cast<TownHall*>(halls[pStru]));  
                return action;
            }

            case RECRUIT:{
                std::uniform_int_distribution<std::mt19937::result_type> unTypeRng(0, NR_OF_UNITS - 1);
                std::vector<Structure*> barracks;
                for (auto& p : pl.structures){
                    if (p->is == BARRACK)
                        barracks.push_back(p.get());
                }
                if (barracks.size() <= 0)
                    return EmptyAction();
                std::uniform_int_distribution<std::mt19937::result_type> struType(0, barracks.size() - 1);
                UnitType unType = static_cast<UnitType>(unTypeRng(rng));
                int barrackIndex = struType(rng);
                RecruitAction action = RecruitAction(unType, barracks[barrackIndex]);
                return action;
            }
            case EMPTY:{
                return EmptyAction();
            }
        }
    }
}

void DQN::Test() {
}

actionT DQN::MapIndexToAction(Player& pl, Player& en, int actionIndex) {
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
        if (targetIndex < MAX_STRUCTS - 1)
        {
            if (targetIndex >= en.structures.size())
                return EmptyAction();
            return AttackAction(pl.units[playerUnit].get(), en.structures[targetIndex].get());
        }
        else
        {
            if (targetIndex >= en.units.size())
                return EmptyAction();
            return AttackAction(pl.units[playerUnit].get(), en.units[targetIndex].get());
        }
    }
    else if (actionIndex < buildAction)
    {
        int offset = actionIndex - attackAction;
        int unit = offset / (NR_OF_STRUCTS * MAP_SIZE * MAP_SIZE);
        StructureType struSelect = static_cast<StructureType>((offset / MAP_SIZE * MAP_SIZE) % NR_OF_STRUCTS);
        int mapSelect = offset % (MAP_SIZE * MAP_SIZE);
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        if (unit >= pl.units.size()){
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
        if (pl.units[peasantIndex]->is != PEASANT)
            return EmptyAction();
        if (pl.structures[hallIndex]->is != HALL)
            return EmptyAction();
        return FarmGoldAction(pl.units[peasantIndex].get(), Vec2(row, col), static_cast<TownHall*>(pl.structures[hallIndex].get()));
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

State DQN::GetState(Player& pl, Player& en, Map& map) {
    State state;
    state.playerGold = pl.gold;
    state.playerFood.x = pl.food.x;
    state.playerFood.y = pl.food.y; 
    state.enemyGold = en.gold; 
    state.enemyFood.x = en.food.x;
    state.enemyFood.y = en.food.y;
    state.playerUnits.resize(pl.units.size());
    state.enemyUnits.resize(en.units.size());
    state.playerStructs.resize(pl.structures.size());
    state.enemyStructs.resize(en.structures.size());
    
    for (int i = 0; i < pl.units.size(); i++)
        state.playerUnits[i] = pl.units[i]->Clone();

    for (int i = 0; i < pl.structures.size(); i++)
        state.playerStructs[i] = pl.structures[i]->Clone();

    for (int i = 0; i < en.units.size(); i++)
        state.enemyUnits[i] = en.units[i]->Clone();

    for (int i = 0; i < en.structures.size(); i++)
        state.enemyStructs[i] = en.structures[i]->Clone();
    
    return state;
}

void DQN::PrintWeight() {
    std::cout << this->layer1->weight[0][0] << std::endl;
}

void DQN::SaveModel(){
    torch::serialize::OutputArchive archive;
    this->save(archive);
    archive.save_to(model_file);
}

void DQN::LoadModel(){
    torch::serialize::InputArchive archive;
    archive.load_from(model_file);
    this->load(archive);
}

TensorStruct::TensorStruct(State& state, Map& map) {
    currentMap = GetMapTensor(map);
    playerGold = torch::tensor(state.playerGold).view({-1, 1});
    playerFood = GetVec(state.playerFood);
    playerUnits = GetUnitsTensor(state.playerUnits);
    playerStructs = GetStructuresTensor(state.playerStructs);
    
    enemyGold = torch::tensor(state.enemyGold).view({-1, 1});
    enemyFood = GetVec(state.enemyFood);
    enemyUnits = GetUnitsTensor(state.enemyUnits);
    enemyStructs = GetStructuresTensor(state.enemyStructs);
}

torch::Tensor TensorStruct::GetMapTensor(Map& map) {
    std::vector<int> data;
    
    for (const auto &row : map.terrain)
    {
        for (const auto &terrain : row)
        {
            data.push_back((terrain.type));
            data.push_back((terrain.resourceLeft));
            data.push_back(terrain.coord.x);
            data.push_back(terrain.coord.y);
        }
    }
    return torch::tensor(data).view({1, -1});
}
torch::Tensor TensorStruct::GetVec(Vec2 food) {
    std::vector<int> data;
    data.push_back(food.x);
    data.push_back(food.y);
    return torch::tensor(data).view({1, -1});
}

torch::Tensor TensorStruct::GetUnitsTensor(std::vector<Unit *> &units) {
    std::vector<int> data;
    for (const auto &unit : units)
    {
        // Extract health and coordinates from each structure
        data.push_back(static_cast<int>(unit->health));
        data.push_back(unit->coordinate.x);
        data.push_back(unit->coordinate.y);
        data.push_back(unit->is);
        data.push_back(static_cast<int>(unit->attack));
        data.push_back(static_cast<int>(unit->maxHealth));
        data.push_back(static_cast<int>(unit->mana));
        data.push_back(static_cast<int>(unit->maxMana));
    }
    return torch::tensor(data).view({1, -1});
}
torch::Tensor TensorStruct::GetStructuresTensor(std::vector<Structure *> &structures) {
    std::vector<int> structureData;
    for (const auto &structure : structures)
    {
        structureData.push_back(static_cast<int>(structure->health));
        structureData.push_back(structure->coordinate.x);
        structureData.push_back(structure->coordinate.y);
        structureData.push_back(structure->is);
    }
    return torch::tensor(structureData).view({1, -1});
}
torch::Tensor TensorStruct::GetTensor() {
    torch::Tensor paddedUnits = torch::zeros({1, (MAX_UNITS - (playerUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructs = torch::zeros({1, (MAX_STRUCTS - (playerStructs.size(1) / strucVar)) * strucVar});
    torch::Tensor paddedUnitsEnemy = torch::zeros({1, (MAX_UNITS - (enemyUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructsEnemy =
        torch::zeros({1, (MAX_STRUCTS - (enemyStructs.size(1) / strucVar)) * strucVar});
    
    std::vector<torch::Tensor> tensors = {
        playerGold, playerFood, playerUnits,      paddedUnits,  playerStructs,     paddedStructs,
        enemyGold,  enemyFood,  enemyUnits, paddedUnitsEnemy, enemyStructs, paddedStructsEnemy};
    
    torch::Tensor concatenatedTensor = torch::cat(tensors, 1);
    
    return concatenatedTensor;
}
