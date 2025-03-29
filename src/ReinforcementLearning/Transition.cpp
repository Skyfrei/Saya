#include "Transition.h"
#include <deque>

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
    int byte_number = 20;// + puSize + psSize + euSize + esSize + npuSize + npsSize
                        //+ neuSize + nesSize);
    
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

    
    return binary_data;    
}
Transition Transition::DeserializeBinary(std::vector<binary> bin){

}


State::State(){
    currentMap = Map();
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
