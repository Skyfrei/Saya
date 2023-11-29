#pragma once

#include <vector>
#include "Terrain.h"

#define n 15

using Warcraft::Living;

namespace Warcraft::Environment
{
    class Map
    {
        public:
            Map()
            {
                for(int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                    {
                        Terrain temp(i, j);
                        temp.type = GROUND;
                        
                        if (j == 0 && i == 0)
                        {
                            temp.type = GOLD;
                            temp.resourceLeft = 2000;
                        }
                        objects[i][j] = temp;
                    }
                }
            }
        

        public:
            void InitializeTerrain()
            {
                
            }
            
        public:
            std::vector<std::vector<Terrain> > objects;

        
    };
}
