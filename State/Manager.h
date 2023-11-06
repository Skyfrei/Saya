#pragma once

#include "../Map/Map.h"
#include "Player.h"

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
                player.Initialize();
                enemy.Initialize();
                MainLoop();
            }
        public:
            void CheckForMovement();
            void MainLoop();
            float GetTime();
            
        public:
            Player player;
            Player enemy;
            Map map;

            
        
        private:
            auto start = std::chrono::steady_clock::now();






    };
}
