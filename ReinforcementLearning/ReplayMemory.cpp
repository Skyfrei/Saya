#include "ReplayMemory.h"

void ReplayMemory::CreateMemoryState(Map map, Player& player, Player& enemy){
  State st;
  st.currentMap = map;
  st.playerFood = player.food;
  st.playerGold = player.gold;

  st.enemyGold = enemy.gold;
  st.enemyFood = enemy.food;
  for (int i = 0; i < player.units.size(); i++){
    Footman* footman = nullptr;
    Peasant* peasant = nullptr;
    switch(player.units[i]->is){
      case FOOTMAN:
        footman = dynamic_cast<Footman*>(player.units[i].get());
        st.playerUnits.push_back(new Footman(*footman));
        break;
      case PEASANT:
        peasant = dynamic_cast<Peasant*>(player.units[i].get());
        st.playerUnits.push_back(new Peasant(*peasant));
        break;
      default:
        break;
    }
  }
  transition.AddTransition(st);
}