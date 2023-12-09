#pragma once

#include <vector>
#include "Terrain.h"
#include <iostream>

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
                    std::vector<Terrain> terr;
                    for (int j = 0; j < n; j++)
                    {
                        Terrain temp(i, j);
                        temp.type = GROUND;
                        
                        if (j % 5 == 0 && i % 5 == 0)
                        {
                            temp.type = GOLD;
                            temp.resourceLeft = 2000;
                        }
                        terr.push_back(temp);
                        //objects[i][j] = temp;
                    }
                    objects.push_back(terr);
                    terr.clear();
                }
            }
            
        public:
            std::vector<std::vector<Terrain> > objects;

    };
}
