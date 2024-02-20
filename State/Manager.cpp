//
// Created by Klavio Tarka on 13.12.23.
//
#include "Manager.h"

namespace Warcraft::State
{
    Manager::Manager()
    {
        // game start
        map = Map();
        player.Initialize(&map); // Done
        enemy.Initialize(&map); // Done
        
        //MainLoop();
    }

    void Manager::MainLoop()
    {
        player.SetInitialCoordinates(Vec2(8, 2));
        enemy.SetInitialCoordinates(Vec2(MAP_SIZE - 2, MAP_SIZE - 3));

        while((player.HasUnit(PEASANT) && player.HasStructure(HALL)) && (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL))) {
            player.RecruitSoldier(ARCHMAGE);
            std::cout<<player.units.size()<<" ";
        }
    }

    void Manager::CheckForMovement()
    {

    }

    float Manager::GetTime()
    {
        time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff = time - time1;
        return diff.count();
    }

    void Manager::ManageLiving(Player& pl)
    {

        std::cout<<"dead";
    }
}
