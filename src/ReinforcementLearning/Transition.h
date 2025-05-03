#ifndef TRANSITION_H
#define TRANSITION_H

#include <string>
#include <vector>
// #include <deque>
#include "../Race/Structure/Structure.h"
#include "../Race/Unit/Unit.h"
#include "../State/Map.h"
#include "../Tools/Binary.h"
#include "../Tools/Vec2.h"
#include <span>

struct State
{
    State();
    State(const State &s);
    State(const State &s, actionT a);
    ~State();
    // bool operator<(const State &other) const;

    int playerGold;
    Vec2 playerFood;
    std::vector<Unit *> playerUnits;
    std::vector<Structure *> playerStructs;

    int enemyGold;
    Vec2 enemyFood;
    std::vector<Unit *> enemyUnits;
    std::vector<Structure *> enemyStructs;

    actionT action;
};

struct Transition
{
    Transition(State s, actionT act, State n, int index, float r = 0.0);
    Transition();
    std::string Serialize();
    Transition Deserialize(std::string &trans);
    std::vector<binary> SerializeBinary();
    Transition DeserializeBinary(std::vector<binary> &bin);
    Unit *GetUnit(std::span<binary> bin);
    Structure *GetStructure(std::span<binary> bin);
    actionT GetAction(std::span<binary> bin);

    State state;
    State nextState;
    actionT action;
    int actionIndex;
    bool done = false;
    float reward = 0.0f;
};

#endif // !TRANSITION_H
