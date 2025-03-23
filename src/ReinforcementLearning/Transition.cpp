#include "Transition.h"

Transition::Transition(State s, actionT act, State n)
                        : state(s, act), nextState(n), action(act){}

Transition::Transition(){}

std::string Transition::Serialize(){
    std::string result;
    result += std::to_string(state.playerGold) + ",";
    result += std::to_string(state.playerFood.x) + "," + std::to_string(state.playerFood.y) + ",";
    for (int i = 0; i < state.playerUnits.size(); i++)
        result += state.playerUnits[i]->Serialize()+ ",";

    for (int i = 0; i < state.playerStructs.size(); i++)
        result += state.playerStructs[i]->Serialize() + ",";

    result += std::to_string(state.enemyGold) + ",";
    result += std::to_string(state.enemyFood.x) + "," + std::to_string(state.enemyFood.y) + ",";
    for (int i = 0; i < state.enemyUnits.size(); i++)
        result += state.enemyUnits[i]->Serialize() + ",";

    for (int i = 0; i < state.enemyStructs.size(); i++)
        result += state.enemyStructs[i]->Serialize() + ","; 

    result += std::to_string(nextState.playerGold) + ",";
    result += std::to_string(nextState.playerFood.x) + "," + std::to_string(nextState.playerFood.y) + ",";
    for (int i = 0; i < nextState.playerUnits.size(); i++)
        result += nextState.playerUnits[i]->Serialize() + ",";

    for (int i = 0; i < nextState.playerStructs.size(); i++)
        result += nextState.playerStructs[i]->Serialize() + ",";

    result += std::to_string(nextState.enemyGold) + ",";
    result += std::to_string(nextState.enemyFood.x) + "," + std::to_string(nextState.enemyFood.y) + ",";
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
