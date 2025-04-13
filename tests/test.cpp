#include <catch2/catch_all.hpp>
#include "../src/ReinforcementLearning/DQN.h"
#include "../src/ReinforcementLearning/Transition.h"
#include "../src/Race/Unit/Unit.h"
#include "../src/Race/Structure/TownHall.h"
#include "../src/Race/Unit/Peasant.h"
#include "../src/gui/Window.h"
#include "../src/Race/Unit/Action.h"
#include <chrono>
#include <fstream>
#include <random>

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
        s.playerUnits[i]->coordinate.x = 0 + i;
        s.playerUnits[i]->coordinate.y = 0 + i;
    }
    s.enemyStructs.push_back(new TownHall(Vec2(10, 2)));
    for (int i = 0; i < 5; i++) {
        s.enemyUnits.push_back(new Peasant());
        s.enemyUnits[i]->coordinate.x = 10 - i;
        s.enemyUnits[i]->coordinate.y = 10 - i;
    }
    actionT act = MoveAction(s.playerUnits[0], Vec2(3, 4));
    Transition trans(s, act, s);
    DQN obj;  
    for(int i = 0; i < 1000; i++)
        obj.AddExperience(trans);
    auto a = std::chrono::high_resolution_clock::now();
    obj.SaveMemory(); 
    auto b = std::chrono::high_resolution_clock::now();
//    obj.LoadMemory();
    auto c = std::chrono::high_resolution_clock::now();
    std::cout << "Time  to save string replay " << duration_cast<milliseconds>(b-a).count();
    std::cout<<"\n Time to load string replay\n" << duration_cast<milliseconds>(c-b).count();
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
        
    std::random_device ran;
    std::default_random_engine e1(ran());
    std::uniform_int_distribution<int> uniform_dist(0, 100);
    DQN obj;  
    for(int i = 0; i < 1000; i++){
        s.playerStructs.push_back(new TownHall(Vec2(10, 2)));
        for (int i = 0; i < 5; i++) {
            s.playerUnits.push_back(new Peasant());
            s.playerUnits[i]->coordinate.x = uniform_dist(e1);
            s.playerUnits[i]->coordinate.y = uniform_dist(e1);
        }
        s.enemyStructs.push_back(new TownHall(Vec2(10, 2)));

        for (int i = 0; i < 5; i++) {
            s.enemyUnits.push_back(new Peasant());
            s.enemyUnits[i]->coordinate.x = uniform_dist(e1);
            s.enemyUnits[i]->coordinate.y = uniform_dist(e1);
        }
        actionT act = MoveAction(s.enemyUnits[0], Vec2(3, 4));
        Transition trans(s, act, s);
        s.playerUnits.clear();
        s.enemyUnits.clear();
        s.playerStructs.clear();
        s.enemyStructs.clear();
        obj.AddExperience(trans);
    }
    obj.SaveMemoryAsBinary(); 
//    auto b = std::chrono::high_resolution_clock::now();
    obj.LoadMemoryAsBinary();
//    auto c = std::chrono::high_resolution_clock::now();
//
//    std::cout<<duration_cast<milliseconds>(c-b).count() << " ";
    return "";
}

std::string GetRenderStrings(actionT& action){
    int dqn_index = action.index();
    std::string curr;
    switch(dqn_index){
        case 0:{
        }
        case 1:{
            MoveAction m = std::get<MoveAction>(action);
            curr = "Move action\nUnit in (" +std::to_string(m.unit->coordinate.x) + "," + std::to_string(m.unit->coordinate.y) + ")->(" + std::to_string(m.destCoord.x) + "," + std::to_string(m.destCoord.y) + ")";
            break;
        }
        case 2:{
            break;
        }
        case 3:{
            break;
        }
    }
    return curr;
}

bool MapRender(){
    Window win(Vec2(1000, 1000));
    int a;
    DQN obj;
    obj.LoadMemoryAsBinary();
    while(true){
        int b = rand() % 1000;
        Transition t = obj.memory[b];
        std::string dqn_string = GetRenderStrings(t.action);
        std::string ppo_string = GetRenderStrings(t.action);
        win.Render(t.state.playerUnits, t.state.enemyUnits, dqn_string, ppo_string);
        //std::cin>>a;
        //if (a == 0)
        //    exit(0);

    }
    return true;
}


TEST_CASE("Serializing and Deserialzing Replays...", "[ReplaySystem]") {
    //REQUIRE(BinaryReplay() == ""); 
}

TEST_CASE("Runtimes of replay system", "[Replay System]") {
   //REQUIRE(RunTimes() <= 2);
}

TEST_CASE("Rendering of the map", "[Map rendering]") {
    REQUIRE(MapRender());
}
