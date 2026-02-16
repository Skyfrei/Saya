#include "Manager.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Peasant.h"
#include <iostream>


Manager::Manager() : player(map, PLAYER), enemy(map, ENEMY) {

    player.SetInitialCoordinates(Vec2(2, 2));
    enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 4, MAP_SIZE - 4));
    trainerManager.InitializePPO(player, enemy, map);
    trainerManager.ppoPolicy.LoadModel("ppo_policy");
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

void Manager::MainLoop() {
    while (!trainerManager.ShouldResetEnvironment(player, enemy, map))
    {
        trainerManager.ShowInMap(player, enemy, map);
        actionT action = trainerManager.GetActionPPO(player, enemy, map);
        player.TakeAction(action);
        player.CheckUnitActions();

        for (int i = static_cast<int>(player.units.size()) - 1; i >= 0; --i) {
            Unit* un = player.units[i].get();
            CheckForOwnership(player, un, action);
        }
        actionT action2 = trainerManager.GetActionPPO(enemy, player, map);
        enemy.TakeAction(action2);
        enemy.CheckUnitActions();
        for (int i = static_cast<int>(enemy.units.size()) - 1; i >= 0; --i) {
            Unit* un = enemy.units[i].get();
            CheckForOwnership(enemy, un, action);
        }
    }

    if (trainerManager.ShouldResetEnvironment(player, enemy, map)){
        map.Reset();
        player.Reset(PLAYER);
        enemy.Reset(ENEMY);
        std::cout << "Player loss"<<std::endl;
    }else if(trainerManager.ShouldResetEnvironment(enemy, player, map)){
        map.Reset();
        player.Reset(PLAYER);
        enemy.Reset(ENEMY);
        std::cout << "Enemy loss"<<std::endl;
    }

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
