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

struct Transition {
    at::Tensor state;
    at::Tensor nextState;
    int actionIndex;
    float reward;
    bool done;

    Transition(torch::Tensor s, int act_idx, torch::Tensor ns, float r, bool d)
        : state(s), actionIndex(act_idx), nextState(ns), reward(r), done(d) {}
    
    Transition() : actionIndex(0), reward(0.0f), done(false) {}
};
#endif // !TRANSITION_H
