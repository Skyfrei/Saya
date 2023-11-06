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
        public:
            Player player;
            Player enemy;
            Map map;

            float GetTime()
            {
                //return end - start;
                float time = std::chrono::steady_clock::now() - start;
                return time;
            }
        
        private:
            auto start = std::chrono::steady_clock::now();






    };
}
