#pragma once

#include "../Map/Map.h"
#include "Player.h"
#include "../Race/Unit/Unit.h"
#include <chrono>
#include "../Living.h"
#include "../Race/Structure/Structure.h"

namespace Warcraft::State
{
    using namespace Warcraft::Environment;
    using namespace Warcraft;
    using Warcraft::Structures::StructureType;

    class Manager
    {
        public:
            Manager();
        public:
            void MainLoop();
            void ManageLiving(Player& pl);
            float GetTime();
            void CheckForMovement();

        public:
            Player player;
            Player enemy;
            Map map;    
            std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    };
}
