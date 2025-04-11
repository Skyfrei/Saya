#include <catch2/catch_all.hpp>
#include "../src/ReinforcementLearning/DQN.h"
#include "../src/ReinforcementLearning/Transition.h"
#include "../src/Race/Unit/Unit.h"
#include "../src/Race/Structure/TownHall.h"
#include "../src/Race/Unit/Peasant.h"
#include "../src/gui/Window.h"
#include <chrono>
#include <fstream>

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
    actionT act = MoveAction(s.enemyUnits[0], Vec2(3, 4));
    Transition trans(s, act, s);
    DQN obj;  
    for(int i = 0; i < 1000; i++)
        obj.AddExperience(trans);
    obj.SaveMemoryAsBinary(); 
    auto b = std::chrono::high_resolution_clock::now();
    obj.LoadMemoryAsBinary();
    auto c = std::chrono::high_resolution_clock::now();

    std::cout<<duration_cast<milliseconds>(c-b).count() << " ";

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

bool MapRender(){
    Window win(Vec2(1000, 1000));
    int a;
    DQN obj;
    auto b = std::chrono::high_resolution_clock::now();
    obj.LoadMemoryAsBinary();
    auto c = std::chrono::high_resolution_clock::now();
    std::cout<<duration_cast<milliseconds>(c-b).count() << " ";
    std::cout<<obj.memory.size() << "\n\n";
    while(true){
        int a = rand() % 1000;
        std::cout<<a << " \n";
        win.Render(obj.memory[a].state.playerUnits, obj.memory[a].state.enemyUnits);
        std::cin>>a;
        if (a == 0)
            exit(0);

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
