#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../src/ReinforcementLearning/DQN.h"
#include "../src/ReinforcementLearning/Transition.h"
#include "../src/Race/Unit/Unit.h"
#include "../src/Race/Structure/TownHall.h"
#include "../src/Race/Structure/Barrack.h"
#include "../src/Race/Unit/Peasant.h"
#include "../src/gui/Window.h"
#include "../src/Race/Unit/Action.h"
#include "../src/ReinforcementLearning/RlManager.h"
#include "../src/ReinforcementLearning/Reward.h"
#include "../src/ReinforcementLearning/ValueNetwork.h"
#include "../src/Tools/Enums.h"
#include <chrono>
#include <fstream>
#include <random>

int mem_size = 2000;
std::string StringReplay(){
    State s;
    s.enemyFood.x = 2;
    s.enemyFood.y = 10;
    s.playerFood.x = 3;
    s.playerFood.y = 4;
    s.enemyGold = 100;
    s.playerGold = 100;
        
    std::random_device ran;
    std::default_random_engine e1(ran());
    std::uniform_int_distribution<int> uniform_dist(0, 99);
    
    RlManager obj;
    Map m;
    Player pl(m, PLAYER);

    for(int i = 0; i < mem_size;){
        s.playerStructs.push_back(new TownHall(Vec2(1, 2)));
        s.playerStructs.push_back(new Barrack(Vec2(1, 3)));
        for (int j = 0; j < 5; j++) {
            s.playerUnits.push_back(new Peasant());
            s.playerUnits[j]->coordinate.x = uniform_dist(e1);
            s.playerUnits[j]->coordinate.y = uniform_dist(e1);
        }
        s.enemyStructs.push_back(new TownHall(Vec2(8, 2)));
        s.enemyStructs.push_back(new Barrack(Vec2(10, 3)));
        for (int j = 0; j < 5; j++) {
            s.enemyUnits.push_back(new Peasant());
            s.enemyUnits[j]->coordinate.x = uniform_dist(e1);
            s.enemyUnits[j]->coordinate.y = uniform_dist(e1);
        }
        int action_index = obj.targetNet.GetRandomOutputIndex();
        actionT act = obj.targetNet.MapIndexToAction(s, action_index);
        if (std::holds_alternative<EmptyAction>(act)){
            s.playerUnits.clear();
            s.enemyUnits.clear();
            s.playerStructs.clear();
            s.enemyStructs.clear();
            continue;
        }
        
        Transition trans(s, act, s, action_index);
        float r = pl.TakeAction(act); 
        trans.reward = r;
        s.playerUnits.clear();
        s.enemyUnits.clear();
        s.playerStructs.clear();
        s.enemyStructs.clear();
        obj.AddExperience(trans);
        i++;
    }
    auto a = std::chrono::high_resolution_clock::now();
    obj.SaveMemoryAsString(); 
    auto b = std::chrono::high_resolution_clock::now();
    obj.LoadMemoryAsString();
    auto c = std::chrono::high_resolution_clock::now();
    std::cout << duration_cast<milliseconds>(b-a).count();
    std::cout<<"," << duration_cast<milliseconds>(c-b).count()<<std::endl;
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
    std::uniform_int_distribution<int> uniform_dist(0, MAP_SIZE);
    
    RlManager obj;
    Map m;
    Player pl(m, PLAYER);
    for(int i = 0; i < mem_size;){
        s.playerStructs.push_back(new TownHall(Vec2(1, 2)));
        s.playerStructs.push_back(new Barrack(Vec2(1, 3)));
        for (int j = 0; j < 5; j++) {
            s.playerUnits.push_back(new Peasant());
            s.playerUnits[j]->coordinate.x = uniform_dist(e1);
            s.playerUnits[j]->coordinate.y = uniform_dist(e1);
        }
        s.enemyStructs.push_back(new TownHall(Vec2(9, 2)));
        s.enemyStructs.push_back(new Barrack(Vec2(9, 3)));
        for (int j = 0; j < 5; j++) {
            s.enemyUnits.push_back(new Peasant());
            s.enemyUnits[j]->coordinate.x = uniform_dist(e1);
            s.enemyUnits[j]->coordinate.y = uniform_dist(e1);
        }
        int action_index = obj.targetNet.GetRandomOutputIndex();
        actionT act = obj.targetNet.MapIndexToAction(s, action_index);
        if (std::holds_alternative<EmptyAction>(act)){
            s.playerUnits.clear();
            s.enemyUnits.clear();
            s.playerStructs.clear();
            s.enemyStructs.clear();
            continue;
        }
        
        Transition trans(s, act, s, action_index);
        float r = pl.TakeAction(act); 
        trans.reward = r;
        s.playerUnits.clear();
        s.enemyUnits.clear();
        s.playerStructs.clear();
        s.enemyStructs.clear();
        obj.AddExperience(trans);
        i++;
    }
    auto a = std::chrono::high_resolution_clock::now();
    obj.SaveMemoryAsBinary(); 
    auto b = std::chrono::high_resolution_clock::now();
    obj.LoadMemoryAsBinary();
    auto c = std::chrono::high_resolution_clock::now();
    std::cout << duration_cast<milliseconds>(b-a).count();
    std::cout<<","<< duration_cast<milliseconds>(c-b).count();
    return "";
}

std::string GetRenderStrings(actionT& action){
    int dqn_index = action.index();
    std::string curr;
    switch(dqn_index){
        case 0:{
            MoveAction m = std::get<MoveAction>(action);
            curr = "Move action\nUnit in (" +std::to_string(m.unit->coordinate.x) + "," + std::to_string(m.unit->coordinate.y) + ")->(" + std::to_string(m.destCoord.x) + "," + std::to_string(m.destCoord.y) + ")";
            break;
        }
        case 1:{
            AttackAction m = std::get<AttackAction>(action);
            curr = "Attack action\nUnit in (" +std::to_string(m.unit->coordinate.x) + "," + std::to_string(m.unit->coordinate.y) + ")->(" + std::to_string(m.object->coordinate.x) + "," + std::to_string(m.object->coordinate.y) + ")";
            break;
        }
        case 2:{
            BuildAction m = std::get<BuildAction>(action);
            curr = "Attack action\nUnit in (" +std::to_string(m.peasant->coordinate.x) + "," + std::to_string(m.peasant->coordinate.y) + ")->(" + std::to_string(m.coordinate.x) + "," + std::to_string(m.coordinate.y) + ")";
            break;

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
    RlManager obj;
    obj.LoadMemoryAsBinary();
    while(true){
        int b = rand() % 1000;
        Transition t = obj.memory[b];
        std::string dqn_string = GetRenderStrings(t.action);
        std::string ppo_string = GetRenderStrings(t.action);
        win.Render(t.state.playerUnits, t.state.enemyUnits, t.state.playerStructs,
                   t.state.enemyStructs, dqn_string, ppo_string);
        //std::cin>>a;
        //if (a == 0)
        //    exit(0);

    }
    return true;
}

bool DQN_Test(){
    Map map;
    Player player(map, PLAYER);
    Player enemy(map, ENEMY);
    player.SetInitialCoordinates(Vec2(8, 2));
    enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));
    RlManager man;
    man.LoadMemoryAsBinary();
    man.InitializeDQN(player, enemy, map);
    man.TrainDQN(player, enemy, map);
    return true;
}

bool PPO_Test(){
    Map map;
    Player player(map, PLAYER);
    Player enemy(map, ENEMY);
    player.SetInitialCoordinates(Vec2(8, 2));
    enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 2));
    RlManager man;
    man.InitializePPO(player, enemy, map);
    man.TrainPPO(player, enemy, map);
    return true;
}


TEST_CASE("Serializing and Deserialzing Replays as string...", "[StringReplaySystem]") {
    //REQUIRE(StringReplay() == ""); 
}

TEST_CASE("Serializing and Deserialzing Replays...", "[ReplaySystem]") {
    REQUIRE(BinaryReplay() == ""); 
}


TEST_CASE("Runtimes of replay system", "[Replay System]") {
   //REQUIRE(RunTimes() <= 2);
}

TEST_CASE("Rendering of the map", "[Map rendering]") {
    //REQUIRE(MapRender());
}

TEST_CASE("Testing DQN", "[DQN]"){
    //REQUIRE(DQN_Test());
}

TEST_CASE("Testing PPO", "[PPO]"){
    REQUIRE(PPO_Test());
}
