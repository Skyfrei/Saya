#ifndef TRANSITION_H
#define TRANSITION_H

#include <vector> 
#include <string>
#include "../State/Map.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Vec2.h"
#include "../Tools/Binary.h"

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
    Transition(State s, actionT act, State n);
    Transition();
    std::string Serialize();
    Transition Deserialize(std::string& trans);
    std::vector<binary> SerializeBinary();
    Transition DeserializeBinary(std::vector<binary> bin);

    State state;
    State nextState;
    actionT action;
    bool done = false;
    double reward = 0.0;
};

#endif  // !TRANSITION_H
