#ifndef TRANSITION_H
#define TRANSITION_H

#include <map>
#include <vector> 
#include "../State/Map.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Vec2.h"

struct State {
    State(){}
    State(const State& s){
        currentMap = s.currentMap;
        enemyFood = s.enemyFood;
        enemyGold = s.enemyGold;
        enemyUnits = s.enemyUnits;
        enemyStructs = s.enemyStructs;
        playerFood = s.playerFood;
        playerGold = s.playerGold;
        playerUnits = s.playerUnits;
        playerStructs = s.playerStructs;
    }
    State(const State& s, actionT a) : State(s) {
        action = a;
    }
    

 bool operator<(const State& other) const {
    if (currentMap == other.currentMap && action == other.action)
          return false;
    return true;
  }

  Map currentMap;
  int playerGold;
  Vec2 playerFood;
  std::vector<Unit*> playerUnits;
  std::vector<Structure*> playerStructs;

  int enemyGold;
  Vec2 enemyFood;
  std::vector<Unit*> enemyUnits;
  std::vector<Structure*> enemyStructs;

  actionT action;
  double reward = 0.0f;
};

struct Transition {
    Transition(State s, actionT action, State n) : state(s, action), nextState(n){}


    State state;
    State nextState;
    bool done = false;
};

#endif  // !TRANSITION_H
