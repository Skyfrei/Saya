#pragma once

#include <vector>
#include "Terrain.h"
#include <iostream>
#include <map>

#define MAP_SIZE 15

using Warcraft::Living;

namespace Warcraft::Environment
{
    struct Node{
        Node(std::vector<Node*> e, Vec2 l) : edges(e), location(l){}
        
        std::vector<Node*> edges;
        Vec2 location;
    };

    struct Graph{
        Graph(std::vector<std::vector<Terrain> > mat){
            int index = 0;
            for (int i = 0; i < MAP_SIZE; i++){
                for(int j = 0; j < MAP_SIZE; j++){
                    std::vector<Node*> neighbors; 
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dy == 0 && dx == 0) continue; // Skip the current cell because its our cell
                            int neighborY = i + dy;
                            int neighborX = j + dx;

                            // Check if neighbor is within bounds and add it
                            if (neighborY >= 0 && neighborY < MAP_SIZE && neighborX >= 0 && neighborX < MAP_SIZE) {
                                neighbors.push_back(&nodes[neighborY * MAP_SIZE + neighborX]);
                            }
                        }
                    }
                    nodes[index] = Node(neighbors, mat[i][j].coord);
                    index++;
                }
            }

            for(int i = 0; i <nodes.size(); i++)
            {
                std::cout<<nodes[i].location.x << " " << nodes[i].location.y<< " \n";
            }
        }
        std::vector<Node>nodes;
    };

    class Map
    {
        public:
            Map()
            {
                for(int i = 0; i < MAP_SIZE; i++)
                {
                    std::vector<Terrain> tempTerr;
                    for (int j = 0; j <MAP_SIZE; j++)
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
                graph = Graph(objects);
            }
        public:
            std::vector<std::vector<Terrain> > objects;
            Graph graph;
            //std::map<Vec2, std::vector<Living>> mapp;

    };
}
