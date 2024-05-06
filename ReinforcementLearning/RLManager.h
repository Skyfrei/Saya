#ifndef RLMANAGER_H
#define RLMANAGER_H

#include <torch/autograd.h>
#include <torch/nn.h>
#include <torch/optim.h>

#include "../Map/Map.h"
#include "../State/Player.h"

class RLManager {
 public:
  RLManager() {}

  void StartPolicy(Map &m, Player &player, Player &enemy);

 private:
};

#endif
