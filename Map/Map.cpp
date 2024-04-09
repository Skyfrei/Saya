#include "Map.h"

#include "Map/Terrain.h"

Map::Map() {
  std::vector<Terrain> tempTerr;
  for (int i = 0; i < MAP_SIZE; i++) {
    for (int j = 0; j < MAP_SIZE; j++) {
      Terrain temp(i, j);
      temp.type = GROUND;

      if (j % 5 == 0 && i % 5 == 0) {
        temp.type = GOLD;
        temp.resourceLeft = 2000;
      }
      tempTerr.push_back(temp);
      // terrain[i][j] = temp;
    }
    terrain.push_back(tempTerr);
    tempTerr.clear();
  }
  graph = Graph(terrain);
}
void Map::RemoveOwnership(Living *l) {
  Terrain terr = GetTerrainAtCoordinate(l->coordinate);
  terr.onTerrainLiving.erase(
      std::find(terr.onTerrainLiving.begin(), terr.onTerrainLiving.end(), l));
}
void Map::AddOwnership(Living *l, Vec2 v) {
  Terrain terr = GetTerrainAtCoordinate(v);
  terr.onTerrainLiving.push_back(l);
}
std::vector<Living *> Map::GetObjectsAtTerrain(Vec2 v) {
  Terrain t = GetTerrainAtCoordinate(v);
  return t.onTerrainLiving;
}
Terrain &Map::GetTerrainAtCoordinate(Vec2 v) {
  for (int i = 0; i < terrain.size(); i++) {
    for (int j = 0; j < terrain.size(); j++) {
      if (terrain[i][j].coord.x == v.x && terrain[i][j].coord.y == v.y) {
        return terrain[i][j];
      }
    }
  }
  return terrain[0][0];
}
void Map::UpdateTerrain(Living &obj) {
  for (int i = 0; i < terrain.size(); i++) {
    for (int j = 0; j < terrain.size(); j++) {
      // if (std::find(terrain[i][j].onTerrainLiving.begin(),
      //             terrain[i][j].onTerrainLiving.end(),
      //           obj) != terrain[i][j].onTerrainLiving.end())
    }
  }
}

std::vector<Node *> Map::GetAllNodes() { return graph.GetAllGraphNodes(); }

std::vector<Node *> Map::GetClosestDestNode(Vec2 &coord, Vec2 &dest) {
  std::vector<Node *> result;
  std::vector<Node *> q;
  std::map<Node *, int> dist;
  std::map<Node *, Node *> prev;
  for (Node *n : GetAllNodes()) {
    dist[n] = 100;
    if (n->location == coord) dist[n] = 0;
    q.push_back(n);
  }

  while (q.size() > 0) {
    auto i_node = std::min_element(
        q.begin(), q.end(),
        [&dist](Node *n1, Node *n2) { return dist[n1] < dist[n2]; });
    Node *u = *i_node;
    q.erase(i_node);
    for (Node *n : graph.GetAllNeighbors(*u)) {
      if (std::find(q.begin(), q.end(), n) != q.end()) {
        int alt = dist[u] + 1;
        std::cout << alt;
        if (alt < dist[n]) {
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
