#include <catch2/catch_all.hpp>
#include "../src/ReinforcementLearning/DQN.h"
#include "../src/ReinforcementLearning/Transition.h"
#include "../src/Race/Unit/Unit.h"
#include "../src/Race/Structure/TownHall.h"
#include "../src/Race/Unit/Peasant.h"

TEST_CASE("Parsing and saving transition works", "[DQN]") {
    State s;
    s.enemyFood.x = 2;
    s.enemyFood.y = 10;
    s.playerFood.x = 3;
    s.playerFood.y = 4;
    s.enemyGold = 100;
    s.playerGold = 100;
    s.playerStructs.push_back(new TownHall());
    for (int i = 0; i < 5; i++) {
        s.playerUnits.push_back(new Peasant());
        s.playerUnits[i]->coordinate.x = 0;
        s.playerUnits[i]->coordinate.y = 0;
    }
    s.enemyStructs.push_back(new TownHall());
    for (int i = 0; i < 5; i++) {
        s.enemyUnits.push_back(new Peasant());
        s.enemyUnits[i]->coordinate.x = 10;
        s.enemyUnits[i]->coordinate.y = 10;
    }
    actionT act = MoveAction(Vec2(3, 4));
    Transition trans(s, act, s);
    DQN obj;  // Create an instance of your class
    // Test cases
    obj.AddExperience(trans);
    //obj.SaveMemory(); 
}
