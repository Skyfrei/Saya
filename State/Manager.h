#pragma once

#include "../Map/Map.h"
#include "Player.h"
#include "../Race/Unit/Unit.h"
#include <chrono>
#include "../Living.h"
#include "../Race/Structure/Structure.h"


using namespace Warcraft::Environment;
using namespace Warcraft;
using Warcraft::Structures::StructureType;


namespace Warcraft::State
{
    class Manager
    {
        public:
            Manager()
            {
                // game start
                player.Initialize(); // Done 
                enemy.Initialize(); // Done
                
                
                MainLoop();
            }
        public:
            void MainLoop()
            {   
                player.SetInitialCoordinates(Vec2(8, 2));
                enemy.SetInitialCoordinates(Vec2(n - 2, n - 3));
                
                while((player.HasUnit(PEASANT) && player.HasStructure(HALL)) && (enemy.HasUnit(PEASANT) && enemy.HasStructure(HALL)))
                {
                    //player.RecruitSoldier(FOOTMAN);
                    //std::cout<<player.units.size()<<" ";
                    //player.units[0]->Attack(*enemy.units[0]);
                    //std::cout<<enemy.units[0]->health;
                    
                    //auto& hall = player.FindClosestLiving(player.units[0], HALL);
                    float time = GetTime();
                    std::cout<<time<<" ";
                }
            }

            void ManageLiving(Player& pl)
            {

                std::cout<<"dead";
            }

            void CheckForMovement()
            {

            }

            float GetTime()
            {
                time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> diff = time - time1;
                return diff.count();
            }
            
        public:
            Player player;
            Player enemy;
            Map map;    
        
        private:
            std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    };
}
