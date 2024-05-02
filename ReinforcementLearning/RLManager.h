#ifndef RLMANAGER_H
#define RLMANAGER_H

#include "../Map/Map.h"
#include "../State/Player.h"

class RLManager {
 public:
  RLManager() {}

  void StartPolicy(Map &m, Player &player, Player &enemy);

 private:
};

#endif
