#ifndef TRANSITION_H

#define TRANSITION_H

#include <map>

#include "Action.h"
#include "State.h"

struct StateAction {
  State state;
  Action action;
};

struct NextState {
  State state;
  float reward = 0.0f;
};

struct Transition {
  std::map<StateAction, NextState> trans;
};

#endif  // !TRANSITION_H
