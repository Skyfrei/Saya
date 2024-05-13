#ifndef RLMANAGER_H
#define RLMANAGER_H

#include "../Map/Map.h"
#include "../State/Player.h"
#include "DQN.h"

class RlManager {
 public:
  RlManager(){}

  void StartPolicy(Map m, Player &player, Player &enemy);
  
  private:
    ReplayMemory memory;
};

#endif
