#pragma once

#include <vector>
#include "Terrain.h"
#include <iostream>
#include <map>

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
                    std::vector<Terrain> tempTerr;
                    for (int j = 0; j < n; j++)
                    {
                        Terrain temp(i, j);
                        temp.type = GROUND;

                        if (j % 5 == 0 && i % 5 == 0)
                        {
                            temp.type = GOLD;
                            temp.resourceLeft = 2000;
                        }
                        tempTerr.push_back(temp);
                        //objects[i][j] = temp;
                    }
                    objects.push_back(tempTerr);
                    tempTerr.clear();
                }
            }

//        std::vector<Living>& GetLivingInTerrain(Vec2 coords)
//        {
//                return mapp.at(coords);
//        }

        void ChangeLivingInTerrain()
        {

        }
            
        public:
            std::vector<std::vector<Terrain> > objects;
            //std::map<Vec2, std::vector<Living>> mapp;

    };
}
