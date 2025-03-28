#include "Transition.h"

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
    result += std::to_string(nextSta.playerStructs.size()) + ",";
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
    int unit_start = 0;
    int unit_index = 0;
    int struct_start = 0;
    int struct_index = 0;
    Transition a;
    for (int i = 0; i < trans.length(); i++){
        if (trans[i] != ','){
            current += trans[i];
            continue;
        }
        if (count <= 11){
            int number = std::stoi(current);
        }
        if (count == 12){
            int unit_number = std::stoi(current);
            unit_index = unit_number * 5;
            unit_start = count + 1;
        }



        
         
        
         
        count++;
        current = "";
    }
   return a; 
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
