#ifndef TRANSITION_H
#define TRANSITION_H

#include <map>
#include <vector> 
#include "../Map/Map.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Vec2.h"

struct State {
  Map currentMap;
  int playerGold;
  Vec2 playerFood;
  std::vector<Unit*> playerUnits;
  std::vector<Structure*> playerStructs;

  int enemyGold;
  Vec2 enemyFood;
  std::vector<Unit*> enemyUnits;
  std::vector<Structure*> enemyStructures;
};

struct StateAction {
  StateAction(State s) {
    state.currentMap = s.currentMap;
    state.enemyFood = s.enemyFood;
    state.enemyGold = s.enemyGold;
    state.enemyUnits = s.enemyUnits;
    state.enemyStructures = s.enemyStructures;
    state.playerFood = s.playerFood;
    state.playerGold = s.playerGold;
    state.playerUnits = s.playerUnits;
    state.playerStructs = s.playerStructs;
  }

  State state;
  actionT typeOfAction;
  bool operator<(const StateAction& other) const {
    if (state.currentMap == other.state.currentMap &&
        typeOfAction == other.typeOfAction){
          return false;
        }
    return true;
  }
};

struct NextState {
  State state;
  float reward = 0.0f;
};

struct Transition {
  void AddTransition(State s){
    StateAction st(s);
    NextState a;
    a.reward = 1.0f;
    trans[st] = a;
  }
  std::map<StateAction, NextState> trans;
};

#endif  // !TRANSITION_H