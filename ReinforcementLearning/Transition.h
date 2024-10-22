#ifndef TRANSITION_H
#define TRANSITION_H

#include <map>
#include <vector> 
#include "../State/Map.h"
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
  StateAction(State s, actionT a) {
    state.currentMap = s.currentMap;
    state.enemyFood = s.enemyFood;
    state.enemyGold = s.enemyGold;
    state.enemyUnits = s.enemyUnits;
    state.enemyStructures = s.enemyStructures;
    state.playerFood = s.playerFood;
    state.playerGold = s.playerGold;
    state.playerUnits = s.playerUnits;
    state.playerStructs = s.playerStructs;
    action = a;
  }

  State state;
  actionT action;
  bool operator<(const StateAction& other) const {
    if (state.currentMap == other.state.currentMap &&
        action == other.action){
          return false;
        }
    return true;
  }
};

struct NextState {
  NextState(State s, float r) : state(s), reward(r){}
  State state;
  float reward = 0.0f;
};

struct Transition {
  Transition(State s, actionT action, NextState n) : stateAction(s, action), nextState(n){}

  StateAction stateAction;
  NextState nextState;
};

#endif  // !TRANSITION_H
