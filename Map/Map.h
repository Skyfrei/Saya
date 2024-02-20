#pragma once

#include <vector>
#include "Terrain.h"
#include <iostream>
#include <map>
#include "../Tools/Vec2.h"


#define MAP_SIZE 15

using Warcraft::Living;

namespace Warcraft::Environment
{
    struct Node{
        Node(std::vector<Node*> e) : neighbors(e){}
        Node(Vec2 l) : location(l){}
        void AddNeighbors(std::vector<Node*> e){
            neighbors = e;
        };
        Node(){}
        std::vector<Node*> neighbors;
        Vec2 location;
    };

    struct Graph{
        Graph(std::vector<std::vector<Terrain> > m){
            // for (int i = 0; i < MAP_SIZE; i++){
            //     for(int j = 0; j < MAP_SIZE; j++){
            //         std::vector<Node*> neighbors; 
            //         Node temp(Vec2(i, j));
            //         for (int dy = -1; dy <= 1; ++dy) {
            //             for (int dx = -1; dx <= 1; ++dx) {
            //                 if (dy == 0 && dx == 0) continue; // Skip the current cell because its our cell
            //                 int neighborY = i + dy;
            //                 int neighborX = j + dx;

            //                 // Check if neighbor is within bounds and add it
            //                 if (neighborY >= 0 && neighborY < MAP_SIZE && neighborX >= 0 && neighborX < MAP_SIZE) {
            //                     neighbors.push_back(new Node(Vec2(neighborX, neighborY)));
            //                 }
            //             }
            //         }
            //         temp.AddNeighbors(neighbors);
            //         nodes[temp.location] = temp;
            //     }
            // }
        }
        Graph(){}
        std::map<Vec2, Node> nodes;  
    };

    class Map{
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
                CreateGraph();
            }

            void CreateGraph()
            {
                Graph g(objects);
                graph = &g;
            }

        public:
            Graph* graph;
            std::vector<std::vector<Terrain> > objects;
    };
}
