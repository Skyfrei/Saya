#include <Player.h>
#include "../Map/Map.h"
#include "../Game.h"

using namespace Warcraft::Environment;

namespace Warcraft::State
{
    class Manager
    {
        public:
            Manager()
            {
                // game start
                game.Initialize();
                map.InitializeTerrain();
                player.Initialize();
                enemy.Initialize();

            }
        public:
            void CheckForMovement();
            void MainLoop();
        public:
            Player player;
            Player enemy;
            Game game;
            Map map;
    };
}
