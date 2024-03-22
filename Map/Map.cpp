#include "Map.h"

#include <cmath>

#include "Graph.h"
#include "Terrain.h"

Map::Map() : graph(objects) {
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
      // objects[i][j] = temp;
    }
    objects.push_back(tempTerr);
    tempTerr.clear();
  }
}

std::vector<Node> Map::GetAllNodes() {
  std::vector<Node> unvisitedNodes = graph.GetAllGraphNodes();
  return unvisitedNodes;
}

std::vector<Node> Map::GetClosestDestNode(Vec2 &coord, Vec2 &dest) {
  std::vector<Node> result;
  std::vector<Node> q;
  std::map<Node, int> dist;
  std::map<Node, Node> prev;

  for (Node n : GetAllNodes()) {
    dist[n] = 100;
    if (n.location == coord) dist[n] = 0;
    q.push_back(n);
  }

  while (q.size() > 0) {
    std::sort(q.begin(), q.end(),
              [&dist](Node &n1, Node &n2) { return dist[n1] < dist[n2]; });
    Node u = q[0];
    q.erase(q.begin());

    for (Node *n : graph.GetAllNeighbors(u)) {
      if (std::find(q.begin(), q.end(), *n) != q.end()) {
        int alt = dist[u] + 1;
        if (alt < dist[*n]) {
          dist[*n] = alt;
          prev[*n] = u;
        }
      }
    }
  }
  Vec2 temp;
  while (temp != coord) {
    Node n = prev[graph.GetNodeAtLocation(dest)];
    if (n.location != coord) {
      result.push_back(n);
    }
    std::cout << n.location.x;
    temp = n.location;
  }

  return result;
}
