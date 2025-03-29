#include <catch2/catch_all.hpp>
#include "../src/ReinforcementLearning/DQN.h"
#include "../src/ReinforcementLearning/Transition.h"
#include "../src/Race/Unit/Unit.h"
#include "../src/Race/Structure/TownHall.h"
#include "../src/Race/Unit/Peasant.h"

std::string StringReplay(){
    State s;
    s.enemyFood.x = 2;
    s.enemyFood.y = 10;
    s.playerFood.x = 3;
    s.playerFood.y = 4;
    s.enemyGold = 100;
    s.playerGold = 100;
    s.playerStructs.push_back(new TownHall(Vec2(10, 2)));
    for (int i = 0; i < 5; i++) {
        s.playerUnits.push_back(new Peasant());
        s.playerUnits[i]->coordinate.x = 0;
        s.playerUnits[i]->coordinate.y = 0;
    }
    s.enemyStructs.push_back(new TownHall(Vec2(10, 2)));
    for (int i = 0; i < 5; i++) {
        s.enemyUnits.push_back(new Peasant());
        s.enemyUnits[i]->coordinate.x = 10;
        s.enemyUnits[i]->coordinate.y = 10;
    }
    actionT act = MoveAction(s.playerUnits[0], Vec2(3, 4));
    Transition trans(s, act, s);
    DQN obj;  
    for(int i = 0; i < 1000; i++)
        obj.AddExperience(trans);
    obj.SaveMemory(); 
    obj.LoadMemory();
    return "";
}

std::string BinaryReplay(){
    State s;
    s.enemyFood.x = 2;
    s.enemyFood.y = 10;
    s.playerFood.x = 3;
    s.playerFood.y = 4;
    s.enemyGold = 100;
    s.playerGold = 100;
    s.playerStructs.push_back(new TownHall(Vec2(10, 2)));
    for (int i = 0; i < 5; i++) {
        s.playerUnits.push_back(new Peasant());
        s.playerUnits[i]->coordinate.x = 0;
        s.playerUnits[i]->coordinate.y = 0;
    }
    s.enemyStructs.push_back(new TownHall(Vec2(10, 2)));
    for (int i = 0; i < 5; i++) {
        s.enemyUnits.push_back(new Peasant());
        s.enemyUnits[i]->coordinate.x = 10;
        s.enemyUnits[i]->coordinate.y = 10;
    }
    actionT act = MoveAction(s.playerUnits[0], Vec2(3, 4));
    Transition trans(s, act, s);
    DQN obj;  
    //for(int i = 0; i < 1000; i++)
    //    obj.AddExperience(trans);
    //obj.SaveMemoryAsBinary(); 
    obj.LoadMemoryAsBinary();

    return "";
}

TEST_CASE("Serializing and Deserialzing Replays...", "[ReplaySystem]") {
//    REQUIRE(StringReplay() == "");
    REQUIRE(BinaryReplay() == ""); 
    //SaveBinary();
}
