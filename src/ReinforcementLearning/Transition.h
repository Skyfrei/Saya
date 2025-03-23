#ifndef TRANSITION_H
#define TRANSITION_H

#include <vector> 
#include <string>
#include "../State/Map.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Vec2.h"


struct State {
    State();
    State(const State& s);
    State(const State& s, actionT a);
    bool operator<(const State& other) const;

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
};

struct Transition {
    Transition(State s, actionT action, State n);
    Transition();
    std::string Parse();
    State state;
    State nextState;
    bool done = false;
    double reward = 0.0;
    
};

#endif  // !TRANSITION_H
