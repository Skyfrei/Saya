#include "RlManager.h"

void RlManager::StartPolicy(Map &map, Player &player, Player &enemy) {
  memory.CreateMemoryState(map, player, enemy);
}
