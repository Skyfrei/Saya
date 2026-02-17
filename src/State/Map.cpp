#include "Map.h"
#include "../Race/Structure/Structure.h"
#include "../Tools/Macro.h"
#include "Terrain.h"
#include <iostream>

Map::Map() {
    std::vector<Terrain> tempTerr;
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            Terrain temp(i, j);
            temp.type = GROUND;

            if (j % 5 == 0 && i % 5 == 0)
            {
                temp.type = GOLD;
                temp.resourceLeft = MAX_MINE_GOLD;
            }
            tempTerr.push_back(temp);
        }
        terrain.push_back(tempTerr);
        tempTerr.clear();
    }
    // graph = Graph(terrain);
}
void Map::Reset(){
    for (auto &row : terrain) {
        for (auto &t : row) {
            t.onTerrainLiving.clear(); 
            t.structureOnTerrain = nullptr;

            if (t.coord.x % 5 == 0 && t.coord.y % 5 == 0)
            {
                t.resourceLeft = MAX_MINE_GOLD;
            }
        }
    }
}

bool Map::operator==(const Map &other) const {
    for (int i = 0; i < terrain.size(); i++)
    {
        for (int j = 0; j < terrain.size(); j++)
        {
            if (terrain[i][j] != other.terrain[i][j])
            {
                return false;
            }
        }
    }
    return true;
}
void Map::RemoveOwnership(Living *l, Vec2 v) {
    Terrain &terr = GetTerrainAtCoordinate(v);

    if (auto s = dynamic_cast<Structure *>(l))
    {
        terr.structureOnTerrain = nullptr;
    }
    else{
        std::erase(terr.onTerrainLiving, l);
    }
}
void Map::AddOwnership(Living *l) {
    Terrain &terr = GetTerrainAtCoordinate(l->coordinate);

    if (auto s = dynamic_cast<Structure *>(l))
    {
        terr.structureOnTerrain = s;
    }
    else
        terr.onTerrainLiving.push_back(l);
}
std::vector<Living *> Map::GetObjectsAtTerrain(Vec2 v) {
    Terrain &t = GetTerrainAtCoordinate(v);
    return t.onTerrainLiving;
}
Terrain &Map::GetTerrainAtCoordinate(Vec2 v) {
    int x = static_cast<int>(v.x);
    int y = static_cast<int>(v.y);
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        std::cout<<x<< " "<<y<< " "<<std::endl;
        throw std::out_of_range("Tried to access outside map!");
    }
    return terrain[x][y];
}


std::vector<Node *> Map::GetAllNodes() {
    return graph.GetAllGraphNodes();
}

std::vector<Node *> Map::GetClosestDestNode(Vec2 &coord, Vec2 &dest) {
    std::vector<Node *> result;
    std::vector<Node *> q;
    std::map<Node *, int> dist;
    std::map<Node *, Node *> prev;
    for (Node *n : GetAllNodes())
    {
        dist[n] = 100;
        if (n->location == coord)
            dist[n] = 0;
        q.push_back(n);
    }

    while (q.size() > 0)
    {
        auto i_node =
            std::min_element(q.begin(), q.end(), [&dist](Node *n1, Node *n2) {
                return dist[n1] < dist[n2];
            });
        Node *u = *i_node;
        q.erase(i_node);
        for (Node *n : graph.GetAllNeighbors(*u))
        {
            if (std::find(q.begin(), q.end(), n) != q.end())
            {
                int alt = dist[u] + 1;
                if (alt < dist[n])
                {
                    dist[n] = alt;
                    prev[n] = u;
                }
            }
        }
    }
    // std::cout << prev.size();
    // for (int i = 0; i < result.size(); i++)
    //   std::cout << result[i]->location.x << " " << result[i]->location.y <<
    //   "\n";
    return result;
}
