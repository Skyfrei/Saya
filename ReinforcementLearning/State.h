#ifndef STATE_H
#define STATE_H
#include "../Map/Map.h"
#include "../State/Player.h"
struct State {
  Map currentMap;
  Player player;
  Player enemy;
};
#endif  // !STATE_H
