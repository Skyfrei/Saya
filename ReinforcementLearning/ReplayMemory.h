#ifndef REPLAYMEMORY_H
#define REPLAYMEMORY_H

#include <deque>
#include "ReinforcementLearning/Transition.h"
#include "../State/Player.h"
#include "../Map/Map.h"

class ReplayMemory {
 public:
  ReplayMemory() {}
  void CreateMemoryState(Map& map, Player& player, Player& enemy);
  void Push(Transition &t) {  }

  std::deque<Transition> transitions;
};

#endif
