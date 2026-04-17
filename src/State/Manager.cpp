#include "Manager.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Peasant.h"
#include <iostream>

extern std::string get_latest_model(const std::string& directory_path, std::string file_start, std::string ext);

Manager::Manager() : player(map, PLAYER), enemy(map, ENEMY) {
    player.SetInitialCoordinates(Vec2(2, 2));
    enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 4, MAP_SIZE - 4));
    trainerManager.InitializePPO(player, enemy, map);
    trainerManager.ppoPolicy.LoadModel(get_latest_model("models/player_model_ppo/", "ppo_policy-", "pt"));
    trainerManager.enemyPPO.LoadModel(get_latest_model("models/enemy_models_ppo/", "ppo_policy-", "pt"));
    DeathManager::Init(&player, &enemy);
}

void Manager::CheckForOwnership(Player &p, Living *l, actionT actionTaken) {
    if (std::holds_alternative<AttackAction>(actionTaken))
    {
        AttackAction &action = std::get<AttackAction>(actionTaken);

        if (action.object != nullptr && action.object->health <= 0)
        {
            map.RemoveOwnership(action.object, action.object->coordinate);
            if (p.side == PLAYER)
            {
                for (auto it = enemy.units.begin(); it != enemy.units.end();)
                {
                    if (it->get() == dynamic_cast<Unit *>(action.object))
                    {
                        it = enemy.units.erase(it);
                        break;
                    }
                    else
                        ++it;
                }
            }
            else
            {
                for (auto it = player.units.begin(); it != player.units.end();)
                {
                    if (it->get() == dynamic_cast<Unit *>(action.object))
                    {
                        it = player.units.erase(it);
                        break;
                    }
                    else
                        ++it;
                }
            }
        }
        if (l->coordinate.x != action.prevCoord.x ||
            l->coordinate.y != action.prevCoord.y)
        {
            map.RemoveOwnership(l, action.prevCoord);
            map.AddOwnership(l);
        }
    }
    else if (std::holds_alternative<MoveAction>(actionTaken))
    {
        MoveAction &action = std::get<MoveAction>(actionTaken);
        map.RemoveOwnership(l, action.prevCoord);
        map.AddOwnership(l);
    }
    else if (std::holds_alternative<BuildAction>(actionTaken))
    {
        BuildAction &action = std::get<BuildAction>(actionTaken);
        if (l->coordinate.x != action.prevCoord.x ||
            l->coordinate.y != action.prevCoord.y)
        {
            map.RemoveOwnership(l, action.prevCoord);
            map.AddOwnership(l);
        }
    }
    else if (std::holds_alternative<FarmGoldAction>(actionTaken))
    {
        FarmGoldAction &action = std::get<FarmGoldAction>(actionTaken);
        auto s = static_cast<Peasant *>(l);
        if (l->coordinate.x != action.prevCoord.x ||
            l->coordinate.y != action.prevCoord.y)
        {
            map.RemoveOwnership(l, action.prevCoord);
            map.AddOwnership(l);
        }
        if (s->WithinDistance(action.hall->coordinate))
        {
            p.gold += action.gold;
            action.gold = 0;
        }
    }
}

int Manager::MainLoop() {
    const int hallCost = 590;
    auto Reset = [&](Player& p){
        bool hasPeasant = p.HasUnit(PEASANT);
        bool hasHall = p.HasStructure(HALL);

        if (!hasPeasant && !hasHall) return true;
        if (!hasPeasant && p.gold < 55) return true;
        if (hasPeasant && p.gold < hallCost && !hasHall) return true;
    
        return false;
    };

    while (!trainerManager.ShouldResetEnvironment(player, enemy, map))
    {
        actionT action = trainerManager.GetActionPPO(player, enemy, map);
        player.TakeAction(action);

        actionT action2 = trainerManager.GetActionPPOEnemy(enemy, player, map);
        enemy.TakeAction(action2);
    }

    if(trainerManager.ShouldResetEnvironment(player, enemy, map)){
      int winner = 0;
        if (Reset(player)){
            std::cout<< "Enemy wins"<<std::endl;
            winner = 2;
        }else{
            std::cout<< "Player wins"<<std::endl;
            winner = 1;
        }
        map.Reset();
        player.Reset(player.side);
        enemy.Reset(enemy.side);
        return winner;
    }
    return 0;
}

int Manager::MainLoopRandom() {
    const int hallCost = 590;
    auto Reset = [&](Player& p){
        bool hasPeasant = p.HasUnit(PEASANT);
        bool hasHall = p.HasStructure(HALL);

        if (!hasPeasant && !hasHall) return true;
        if (!hasPeasant && p.gold < 55) return true;
        if (hasPeasant && p.gold < hallCost && !hasHall) return true;
    
        return false;
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    while (!trainerManager.ShouldResetEnvironment(player, enemy, map))
    {
        if (dist(gen) == 0) {
            actionT action = trainerManager.GetActionPPO(player, enemy, map);
            player.TakeAction(action);

            actionT action2 = trainerManager.GetActionPPOEnemy(enemy, player, map);
            enemy.TakeAction(action2);
        } else {
            actionT action2 = trainerManager.GetActionPPOEnemy(enemy, player, map);
            enemy.TakeAction(action2);

            actionT action = trainerManager.GetActionPPO(player, enemy, map);
            player.TakeAction(action);
        }
    }

    if(trainerManager.ShouldResetEnvironment(player, enemy, map)){
        int winner = 0;
        if (Reset(player)){
            winner = 2;
        } else {
            winner = 1;
        }
        
        map.Reset();
        player.Reset(player.side);
        enemy.Reset(enemy.side);
        return winner;
    }
    return 0; 
}

void Manager::CheckForMovement() {
}

void Manager::PrintVector(Vec2 a) {
    std::cout << "\n" << a.x << " " << a.y << "\n";
}

bool Manager::Is10thSecond() {
    auto currentCd = std::chrono::duration_cast<std::chrono::milliseconds>(
        high_resolution_clock::now() - frameTimer);

    if (currentCd >= frameCooldown)
    {
        frameTimer = std::chrono::high_resolution_clock::now();
        return true;
    }
    return false;
}

float Manager::GetTime() {
    time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = time - time1;
    return diff.count();
}

void Manager::ManageLiving(Player &pl) {
    std::cout << "dead";
}
