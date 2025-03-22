#include "Transition.h"

Transition::Transition(State s, actionT action, State n)
                        : state(s, action), nextState(n){}

Transition::Transition(){}

std::string Transition::Parse(){
    std::string result;
    result += std::to_string(state.playerGold) + " ";
    result += std::to_string(state.playerFood.x) + " " + std::to_string(state.playerFood.y) + " ";
    for (int i = 0; i < state.playerUnits.size(); i++){
        int type = static_cast<int>(state.playerUnits[i]->is);
        float health = state.playerUnits[i]->health;
        float mana = state.playerUnits[i]->mana;
        int x = state.playerUnits[i]->coordinate.x;
        int y = state.playerUnits[i]->coordinate.y;
        result += std::to_string(type) + " " + std::to_string(health) + " " + std::to_string(mana) + " " + std::to_string(x) + " " + std::to_string(y) + " ";
    }
    for (int i = 0; i < state.playerStructs.size(); i++){
        int type = static_cast<int>(state.playerStructs[i]->is);
        float health = state.playerStructs[i]->health;
        int x = state.playerStructs[i]->coordinate.x;
        int y = state.playerStructs[i]->coordinate.y;
        result += std::to_string(type) + " " + std::to_string(health) + " " + std::to_string(x) + " " + std::to_string(y) + " ";
    }

    result += std::to_string(state.enemyGold) + " ";
    result += std::to_string(state.enemyFood.x) + " " + std::to_string(state.enemyFood.y) + " ";
    for (int i = 0; i < state.enemyUnits.size(); i++){
        int type = static_cast<int>(state.enemyUnits[i]->is);
        float health = state.enemyUnits[i]->health;
        float mana = state.enemyUnits[i]->mana;
        int x = state.enemyUnits[i]->coordinate.x;
        int y = state.enemyUnits[i]->coordinate.y;
        result += std::to_string(type) + " " + std::to_string(health) + " " + std::to_string(mana) + " " + std::to_string(x) + " " + std::to_string(y) + " ";
    }
    for (int i = 0; i < state.enemyStructs.size(); i++){
        int type = static_cast<int>(state.enemyStructs[i]->is);
        float health = state.enemyStructs[i]->health;
        int x = state.enemyStructs[i]->coordinate.x;
        int y = state.enemyStructs[i]->coordinate.y;
        result += std::to_string(type) + " " + std::to_string(health) + " " + std::to_string(x) + " " + std::to_string(y) + " ";
    }
    return result;
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
