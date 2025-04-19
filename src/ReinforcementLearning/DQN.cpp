#include "DQN.h"
#include <cmath>
#include <fstream>
#include <map>
#include <random>
#include "../Tools/Macro.h"
#include "../Tools/Binary.h"
#include "../Tools/Enum.h"

int mapSize = MAP_SIZE * MAP_SIZE;
int moveAction = mapSize * MAX_UNITS;
int attackAction = moveAction + MAX_UNITS * (MAX_STRUCTS + MAX_UNITS);
int buildAction = attackAction + PEASANT_INDEX_IN_UNITS * NR_OF_STRUCTS * mapSize;
int farmAction =
    buildAction + PEASANT_INDEX_IN_UNITS * mapSize * HALL_INDEX_IN_STRCTS;   // town hall size multipled here as well
int recruitAction = farmAction + 2 * NR_OF_UNITS * BARRACK_INDEX_IN_STRUCTS; // barrack size

DQN::DQN(){
}

void DQN::Initialize(State state) {
    TensorStruct ts(state);
    inputSize = ts.GetTensor().size(1);
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
    //torch::optim::SGD optimizer(this->parameters(), learningRate);
    float epsilon = 0.8f;
    float loss = 0.0f;

    for (int i = 0; i < epochNumber; i++)
    {
        for (int j = 0; j < 1000; j++){
              
            actionT action = SelectAction(pl, en, map);
        }
    }
}

actionT DQN::SelectAction(Player& pl, Player& en, Map& map) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist1(0.0, 1.0);
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

    if (random_number > epsilon){
        at::Tensor action = std::get<1>(Forward(TurnStateInInput(state)).max(1)).view({1, 1});
        actionT result = MapIndexToAction(action.item<int>());
        return result;
    }
    else{
        ActionType action_index = static_cast<ActionType>(dist2(rng));
        
        switch(action_index){
            case MOVE:{
                MoveAction action = MoveAction(pl.units[pUnit], Vec2(cx, cy));
                return action;
            }

            case ATTACK:{
                std::uniform_int_distribution<std::mt19937::result_type> struOrEn(0, 1);
                int ran = struOrEn(rng);
                if (ran == 0){
                    AttackAction action = AttackAction(pl.units[pUnit], en.units[eUnit]);
                    return action;
                }
                else{
                    AttackAction action = AttackAction(pl.units[pUnit], en.structures[eStru]);
                    return action;
                }
            }

            case BUILD:{
                std::uniform_int_distribution<std::mt19937::result_type> struType1(0, NR_OF_STRUCTURES - 1);
                StructureType struType = static_cast<StructureType>(struType1(rng));
                std::vector<Units*> peasants;
                for (auto p : pl.units){
                    if (p->Type == PEASANT)
                        peasants.push_back(p);
                }
                if (peasants.size() <= 0)
                    break;
                std::uniform_int_distribution<std::mt19937::result_type> peasantX(0, peasants.size() - 1);
                BuildAction action = BuildAction(pl.peasants[peasantX(rng)], struType, Vec2(cx, cy));
                return action;
            }
            
            case FARMGOLD:{
                std::vector<Units*> peasants;
                std::vector<Structure*> halls;
                for (auto p : pl.units){
                    if (p->Type == PEASANT)
                        peasants.push_back(p);
                }
                for (auto s : pl.structures){
                    if (s->is == HALL)
                        halls.push_back(s);
                }

                if (peasants.size() <= 0)
                    break;
                if (halls.size() <= 0)
                    break;
                std::uniform_int_distribution<std::mt19937::result_type> peasantX(0, peasants.size() - 1);
                std::uniform_int_distribution<std::mt19937::result_type> struX(0, halls.size() - 1);
                pUnit = peasantX(rng);
                pStru = struX(rng);

                FarmGoldAction action = FarmGoldAction(pl.peasants[pUnit], Vec2(cx, cy), pl.structures[pStru]);  
                return action;
            }

            case RECRUIT:{
                std::uniform_int_distribution<std::mt19937::result_type> unTypeRng(0, NR_OF_UNITS - 1);
                std::vector<Structure*> barracks;
                for (auto p : pl.structures){
                    if (p->is == BARRACK)
                        barracks.push_back(p);
                }

                std::uniform_int_distribution<std::mt19937::result_type> struType(0, barracks.size() - 1);
                UnitType unType = static_cast<UnitType>(unTypeRng(rng));
                int barrackIndex = struType(rng);
                RecruitAction action = RecruitAction(unType, p.barracks[barrackIndex]);
                return action;
            }
        }
    }
}

void DQN::Test() {
}
// mapSize = 3 * 3 = 9
// MAP_SIZE = 3
actionT DQN::MapIndexToAction(int actionIndex) {
    actionT action;

    if (actionIndex < moveAction)
    {
        int col = actionIndex % MAP_SIZE;
        int row = (actionIndex / MAP_SIZE) % MAP_SIZE;
        int unitIndex = (actionIndex / (mapSize)) % MAX_UNITS;
        // TODO
    }
    else if (actionIndex < attackAction)
    {
        int offset = actionIndex - moveAction;
        int playerUnit = (offset / (MAX_STRUCTS + MAX_UNITS)) % MAX_UNITS;
        int targetIndex = offset % (MAX_STRUCTS + MAX_UNITS);
        if (targetIndex < MAX_STRUCTS - 1)
        {
            // pass enemy struct
        }
        else
        {
            int temp = targetIndex - MAX_STRUCTS;
            // pass enemy unit
        }
    }
    else if (actionIndex < buildAction)
    {
        int offset = actionIndex - attackAction;
        int unit = offset / (NR_OF_STRUCTS * mapSize);
        int struSelect = (offset / mapSize) % NR_OF_STRUCTS;
        int mapSelect = offset % mapSize;
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        // Pass build action here.
    }
    else if (actionIndex < farmAction)
    {
        int offset = actionIndex - buildAction;
        int peasantIndex = offset / (mapSize * HALL_INDEX_IN_STRCTS);
        int hallIndex = (offset / mapSize) % HALL_INDEX_IN_STRCTS;
        int mapSelect = offset % mapSize;
        int col = mapSelect % MAP_SIZE;
        int row = mapSelect / MAP_SIZE;
        // pass farm action here
    }
    else if (actionIndex < recruitAction)
    {
        int offset = actionIndex - farmAction;
        int unitType = offset / BARRACK_INDEX_IN_STRUCTS;
        int barrackIndex = offset % BARRACK_INDEX_IN_STRUCTS;
        // pass recruitAction here
    }
    return action;
}



torch::Tensor DQN::TurnStateInInput(State state) {
    TensorStruct ts(state);
    return ts.GetTensor();
}

void DQN::PrintWeight() {
    std::cout << this->layer1->weight[0][0] << std::endl;
}
