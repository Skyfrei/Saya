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

//    for (int i = 0 ; i < obj.memory.size(); i++){
//        for (int j = 0; j < s.playerUnits.size(); j++){
//            if (obj.memory[i].state.playerUnits[j]->health != s.playerUnits[j]->health)
//                std::cout<<"not equal";
//            if (obj.memory[i].state.playerUnits[j]->mana != s.playerUnits[j]->mana)
//                std::cout<<"not equal mana";
//            if (obj.memory[i].state.playerUnits[j]->coordinate.x != s.playerUnits[j]->coordinate.x)
//                std::cout<<"not equal x";
//            if (obj.memory[i].state.playerUnits[j]->coordinate.y != s.playerUnits[j]->coordinate.y)
//                std::cout<<"not equal y";
//            if (obj.memory[i].state.playerUnits[j]->is != s.playerUnits[j]->is)
//                std::cout<<"not equal type";
//        }
//
//    }


    return "";
}

TEST_CASE("Serializing and Deserialzing Replays...", "[ReplaySystem]") {
//    REQUIRE(StringReplay() == "");
    REQUIRE(BinaryReplay() == ""); 
    //SaveBinary();
}

