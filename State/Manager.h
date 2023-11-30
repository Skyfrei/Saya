#pragma once

#include "../Map/Map.h"
#include "Player.h"
#include <chrono>

using namespace Warcraft::Environment;

namespace Warcraft::State
{
    class Manager
    {
        public:
            Manager()
            {
                // game start
                map.InitializeTerrain();
                player.Initialize(); // Done 
                enemy.Initialize(); // Done
                MainLoop();
            }
        public:
            void MainLoop()
            {   
                while((player.HasPeasant() && player.HasTownHall()) && (enemy.HasPeasant() && enemy.HasTownHall()))
                {
                    
                }
            }

            void CheckForMovement()
            {

            }

            float GetTime()
            {
                time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::milli> diff = time - time1;
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
