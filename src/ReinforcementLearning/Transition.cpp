#include "Transition.h"
#include <span>
#include "../Tools/Enums.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Structure/Farm.h"

Transition::Transition(State s, actionT act, State n)
                        : state(s, act), nextState(n), action(act){}

Transition::Transition(){}

std::string Transition::Serialize(){
    std::string result;
    result += std::to_string(state.playerGold) + ",";
    result += std::to_string(state.playerFood.x) + "," + std::to_string(state.playerFood.y) + ",";
    result += std::to_string(state.enemyGold) + ",";
    result += std::to_string(state.enemyFood.x) + "," + std::to_string(state.enemyFood.y) + ",";
    result += std::to_string(nextState.playerGold) + ",";
    result += std::to_string(nextState.playerFood.x) + "," + std::to_string(nextState.playerFood.y) + ",";
    result += std::to_string(nextState.enemyGold) + ",";
    result += std::to_string(nextState.enemyFood.x) + "," + std::to_string(nextState.enemyFood.y) + ",";


    result += std::to_string(state.playerUnits.size()) + ",";
    result += std::to_string(state.playerStructs.size()) + ",";
    result += std::to_string(state.enemyUnits.size()) + ",";
    result += std::to_string(state.enemyStructs.size()) + ",";
    result += std::to_string(nextState.playerUnits.size()) + ",";
    result += std::to_string(nextState.playerStructs.size()) + ",";
    result += std::to_string(nextState.enemyUnits.size()) + ",";
    result += std::to_string(nextState.enemyStructs.size()) + ",";


    for (int i = 0; i < state.playerUnits.size(); i++)
        result += state.playerUnits[i]->Serialize()+ ",";
    
    for (int i = 0; i < state.playerStructs.size(); i++)
        result += state.playerStructs[i]->Serialize() + ",";

    for (int i = 0; i < state.enemyUnits.size(); i++)
        result += state.enemyUnits[i]->Serialize() + ",";
    
    for (int i = 0; i < state.enemyStructs.size(); i++)
        result += state.enemyStructs[i]->Serialize() + ","; 

    for (int i = 0; i < nextState.playerUnits.size(); i++)
        result += nextState.playerUnits[i]->Serialize() + ",";

    for (int i = 0; i < nextState.playerStructs.size(); i++)
        result += nextState.playerStructs[i]->Serialize() + ",";

    for (int i = 0; i < nextState.enemyUnits.size(); i++)
        result += nextState.enemyUnits[i]->Serialize() + ",";

    for (int i = 0; i < nextState.enemyStructs.size(); i++)
        result += nextState.enemyStructs[i]->Serialize() + ",";
    
    std::visit([&result](auto& act){
        result += act.Serialize() + "\n";
    }, action);

    result += std::to_string(reward);
    return result;
}

Transition Transition::Deserialize(std::string& trans){
    std::string current = "";
    int count = 0;
    Transition a;
    std::deque<std::string> objs;
    for (int i = 0; i < trans.length(); i++){
        if (trans[i] != ','){
            current += trans[i];
            if (i == trans.length() - 1)
                objs.push_back(current);
            continue;
        }
        objs.push_back(current);
        current = "";
    }
    // 8-15
    int sp_unitSize = std::stoi(objs[12]);
    int sp_structureSize = std::stoi(objs[13]); 
    int se_unitSize = std::stoi(objs[14]);
    int se_structureSize = std::stoi(objs[15]); 
    int np_unitSize = std::stoi(objs[16]);
    int np_structureSize = std::stoi(objs[17]); 
    int ne_unitSize = std::stoi(objs[18]);
    int ne_structureSize = std::stoi(objs[19]); 
    for (int i = 0; i<20; i++)
        objs.pop_front();

    for (int i = 0; i < sp_unitSize; i++){
        std::string un = objs[0] + " " + objs[1] + " " + objs[2] +  " " +objs[3]  + " " + objs[4];
        // deserialize unit on string
        // Unit* u = Factory.Deserialize(un);
        for (int j = 0; j < 5; j++)
            objs.pop_front();
        std::cout<<un << "\n";
    }
    for (int i = 0; i < sp_structureSize; i++){
        std::string stru = objs[0] + " " + objs[1] + " " + objs[2] + " " + objs[3];
        for (int j = 0; j< 4; j++)
             objs.pop_front();
        std::cout<<stru<<"\n";
    }
    for (int i = 0; i < se_unitSize; i++){
        std::string un = objs[0] + " " + objs[1] + " " + objs[2] +  " " +objs[3]  + " " + objs[4];
        for (int j = 0; j < 5; j++)
            objs.pop_front();
        std::cout<<un << "\n";
    }
    for (int i = 0; i < se_structureSize; i++){
        std::string stru = objs[0] + " " + objs[1] + " " + objs[2] + " " + objs[3];
        for (int j = 0; j< 4; j++)
             objs.pop_front();
        std::cout<<stru<<"\n";
    }
    for (int i = 0; i < np_unitSize; i++){
        std::string un = objs[0] + " " + objs[1] + " " + objs[2] +  " " +objs[3]  + " " + objs[4];
        // deserialize unit on string
        // Unit* u = Factory.Deserialize(un);
        for (int j = 0; j < 5; j++)
            objs.pop_front();
        std::cout<<un << "\n";
    }
    for (int i = 0; i < np_structureSize; i++){
        std::string stru = objs[0] + " " + objs[1] + " " + objs[2] + " " + objs[3];
        for (int j = 0; j< 4; j++)
             objs.pop_front();
        std::cout<<stru<<"\n";
    }
    for (int i = 0; i < ne_unitSize; i++){
        std::string un = objs[0] + " " + objs[1] + " " + objs[2] +  " " +objs[3]  + " " + objs[4];
        // deserialize unit on string
        // Unit* u = Factory.Deserialize(un);
        for (int j = 0; j < 5; j++)
            objs.pop_front();
        std::cout<<un << "\n";
    }

    for (int i = 0; i < ne_structureSize; i++){
        std::string stru = objs[0] + " " + objs[1] + " " + objs[2] + " " + objs[3];
        for (int j = 0; j< 4; j++)
             objs.pop_front();
        std::cout<<stru<<"\n";
    }
    
    
    return a; 
}

std::vector<binary> Transition::SerializeBinary(){
    std::vector<binary> binary_data;
    int puSize = state.playerUnits.size();
    int psSize = state.playerStructs.size();
    int euSize = state.enemyUnits.size();        
    int esSize = state.enemyStructs.size();     
    int npuSize = nextState.playerUnits.size();
    int npsSize = nextState.playerStructs.size();
    int neuSize = nextState.enemyUnits.size();      
    int nesSize = nextState.enemyStructs.size();
    int byte_number = 20 + (puSize * 5) + (psSize * 4) + (euSize * 5) + (esSize * 4) + (npuSize * 5) + (npsSize * 4) + (neuSize * 5) + (nesSize * 4);
    
    binary_data.push_back(byte_number);
    binary_data.push_back(state.playerGold);
    binary_data.push_back(state.playerFood.x);
    binary_data.push_back(state.playerFood.y);
    binary_data.push_back(state.enemyGold);
    binary_data.push_back(state.enemyFood.x);
    binary_data.push_back(state.enemyFood.y);
    binary_data.push_back(nextState.playerGold);
    binary_data.push_back(nextState.playerFood.x);
    binary_data.push_back(nextState.playerFood.y);
    binary_data.push_back(nextState.enemyGold);
    binary_data.push_back(nextState.enemyFood.x);
    binary_data.push_back(nextState.enemyFood.y);

    binary_data.push_back(puSize);
    binary_data.push_back(psSize);
    binary_data.push_back(euSize);
    binary_data.push_back(esSize);
    binary_data.push_back(npuSize);
    binary_data.push_back(npsSize);
    binary_data.push_back(neuSize);
    binary_data.push_back(nesSize);

    
    for (int i = 0; i < puSize; i++){
        std::vector<binary> vec = state.playerUnits[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < psSize; i++){
        std::vector<binary> vec = state.playerStructs[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < euSize; i++){
        std::vector<binary> vec = state.enemyUnits[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < esSize; i++){
        std::vector<binary> vec = state.enemyStructs[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < npuSize; i++){
        std::vector<binary> vec = nextState.playerUnits[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < npsSize; i++){
        std::vector<binary> vec = nextState.playerStructs[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < neuSize; i++){
        std::vector<binary> vec = nextState.enemyUnits[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    for (int i = 0; i < nesSize; i++){
        std::vector<binary> vec = nextState.enemyStructs[i]->SerializeBinary();
        binary_data.insert(binary_data.end(), vec.begin(), vec.end());
    }

    std::visit([&binary_data](auto& act){
        std::deque<binary> actionData = act.SerializeBinary();
        binary_data[0] = static_cast<int>(std::get<int>(binary_data[0]) + actionData.size());
        binary_data.insert(binary_data.end(), actionData.begin(), actionData.end());
    }, action);
    
    return binary_data;    
}
Transition Transition::DeserializeBinary(std::deque<binary>& bin){
    State state;
    State nextState;

    state.playerGold = std::get<int>(bin[0]);
    state.playerFood.x = std::get<int>(bin[1]);
    state.playerFood.y = std::get<int>(bin[2]);
    state.enemyGold = std::get<int>(bin[3]);
    state.enemyFood.x = std::get<int>(bin[4]);
    state.enemyFood.y = std::get<int>(bin[5]);
    nextState.playerGold = std::get<int>(bin[6]);
    nextState.playerFood.x = std::get<int>(bin[7]);
    nextState.playerFood.y = std::get<int>(bin[8]); 
    nextState.enemyGold = std::get<int>(bin[9]);
    nextState.enemyFood.x = std::get<int>(bin[10]);  
    nextState.enemyFood.y = std::get<int>(bin[11]);
    int puSize = std::get<int>(bin[12]);
    int psSize = std::get<int>(bin[13]);
    int euSize = std::get<int>(bin[14]);
    int esSize = std::get<int>(bin[15]);
    int npuSize = std::get<int>(bin[16]);
    int npsSize = std::get<int>(bin[17]);
    int neuSize = std::get<int>(bin[18]);
    int nesSize = std::get<int>(bin[19]);
    state.playerUnits.resize(puSize);
    state.enemyUnits.resize(euSize);
    nextState.playerUnits.resize(npuSize);
    nextState.enemyUnits.resize(neuSize);
    state.playerStructs.resize(psSize);
    state.enemyStructs.resize(esSize);
    nextState.playerStructs.resize(npsSize);
    nextState.enemyStructs.resize(nesSize);

    bin.erase(bin.begin(), bin.begin() + 20);

    
    for (int i = 0; i < puSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 5);
        state.playerUnits[i] = GetUnit(package);
        bin.erase(bin.begin(), bin.begin() + 5);
    }

    for (int i = 0; i < psSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 4);
        state.playerStructs[i] = GetStructure(package);
        bin.erase(bin.begin(), bin.begin() + 4);
    }
    
    for (int i = 0; i < euSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 5);
        state.enemyUnits[1] = GetUnit(package);
        bin.erase(bin.begin(), bin.begin() + 5);
    }

    for (int i = 0; i < esSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 4);
        state.enemyStructs[i] = GetStructure(package);
        bin.erase(bin.begin(), bin.begin() + 4);
    }

    for (int i = 0; i < npuSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 5);
        nextState.playerUnits[i] = GetUnit(package);
        bin.erase(bin.begin(), bin.begin() + 5);
    }

    for (int i = 0; i < npsSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 4);
        nextState.playerStructs[i] = GetStructure(package);
        bin.erase(bin.begin(), bin.begin() + 4);
    }
    
    for (int i = 0; i < neuSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 5);
        nextState.enemyUnits[i] = GetUnit(package);
        bin.erase(bin.begin(), bin.begin() + 5);
    }

    for (int i = 0; i < nesSize; i++){
        std::vector<binary> package(bin.begin(), bin.begin() + 4);
        nextState.enemyStructs[i] = GetStructure(package);
        bin.erase(bin.begin(), bin.begin() + 4);
    }

    actionT deserAction = GetAction(bin);
    Transition trans(state, deserAction, nextState);

    return trans;
}

actionT Transition::GetAction(std::deque<binary>& bin){
    int actionType = std::get<int>(bin[0]);
    bin.pop_front();
    switch(actionType){
        case 0:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            Vec2 dest(std::get<int>(bin[5]), std::get<int>(bin[6]));
            MoveAction move(newUnit, dest);
            bin.erase(bin.begin(), bin.begin() + 7);
            return move;
        }

        case 1:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            int unitOrStruct = std::get<int>(bin[5]);
            Unit* targetUnit;
            Structure* targetStructure;
            bin.erase(bin.begin(), bin.begin() + 6);
            if (unitOrStruct == 0){
                std::vector<binary> package2(bin.begin(), bin.begin() + 5);
                targetUnit = GetUnit(package2); 
                delete targetStructure;
                bin.erase(bin.begin(), bin.begin() + 5);
            }else{
                std::vector<binary> package2(bin.begin(), bin.begin() + 4);
                targetStructure = GetStructure(package2); 
                delete targetUnit;
                bin.erase(bin.begin(), bin.begin() + 4);
            }
            AttackAction attackAction(targetUnit, targetStructure);
            return attackAction;
        }

        case 2:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            //bin.erase(bin.begin(), bin.begin() + 5);
            //std::vector<binary> package2(bin.begin(), bin.begin() + 4);
            StructureType structType = static_cast<StructureType>(std::get<int>(bin[5]));
            Vec2 buildCoord(std::get<int>(bin[6]), std::get<int>(bin[7]));
            bin.erase(bin.begin(), bin.begin() + 8);

            Unit* newUnit = GetUnit(package);
            //Structure* newStruct = GetStructure(package2);
            BuildAction buildAction(newUnit, structType, buildCoord);
            return buildAction;
        }

        case 3:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            int x = std::get<int>(bin[5]);
            int y = std::get<int>(bin[6]);
            Vec2 dest(x, y);

            bin.erase(bin.begin(), bin.begin() + 7);
            FarmGoldAction farmAction(newUnit, dest);
            return farmAction;
        }

        case 4:{
            UnitType unitType = static_cast<UnitType>(std::get<int>(bin[0]));
            std::vector<binary> package(bin.begin() + 1, bin.begin() + 4);
            Structure* binStru = GetStructure(package);
            
            bin.erase(bin.begin(), bin.begin() + 5);
            RecruitAction recruitAction(unitType, binStru);
            return recruitAction;
        }
    }
}

Unit* Transition::GetUnit(std::vector<binary>& bin){
    UnitType type = static_cast<UnitType>(std::get<int>(bin[0]));
    float health = std::get<float>(bin[1]);
    float mana = std::get<float>(bin[2]);
    int x = std::get<int>(bin[3]);
    int y = std::get<int>(bin[4]);
    Unit* un;
    switch(type){
        case FOOTMAN:
            un = new Footman(Vec2(x, y), health, mana);
            break;
                                                                       
        case PEASANT:
            un = new Peasant(Vec2(x, y), health, mana);
            break;
    }
    return un;
}

Structure* Transition::GetStructure(std::vector<binary>& bin){
    StructureType type = static_cast<StructureType>(std::get<int>(bin[0]));
    float health = std::get<float>(bin[1]);
    int x = std::get<int>(bin[2]);
    int y = std::get<int>(bin[3]);
    Structure* str;


    switch(type){
        case HALL:
            str = new TownHall(Vec2(x, y), health);
            break;

        case BARRACK:
            str = new Barrack(Vec2(x, y), health);
            break;


        case FARM:
            str = new Farm(Vec2(x, y), health);
            break;
    }

    return str;
}

State::State(){

}

State::State(const State& s){
    enemyFood = s.enemyFood;
    enemyGold = s.enemyGold;
    playerFood = s.playerFood;
    playerGold = s.playerGold;
    currentMap = s.currentMap;
    
    enemyUnits = s.enemyUnits;
    enemyStructs = s.enemyStructs;
    playerUnits = s.playerUnits;
    playerStructs = s.playerStructs;
}

State::State(const State& s, actionT a) : State(s) {
    action = a;
}

bool State::operator<(const State& other) const{
    if (currentMap == other.currentMap && action == other.action)
          return false;
    return true;
}
