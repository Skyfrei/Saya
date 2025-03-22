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
    double reward = 0.0f;
};

struct Transition {
    Transition(State s, actionT action, State n);
    Transition();
    State state;
    State nextState;
    bool done = false;
    std::string Parse();
};

#endif  // !TRANSITION_H
