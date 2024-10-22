#include "Graph.h"

Graph::Graph(std::vector<std::vector<Terrain>> m) {
  for (int i = 0; i < MAP_SIZE; i++) {
    for (int j = 0; j < MAP_SIZE; j++) {
      std::vector<Node *> neighbors;
      Node *temp = &GetNodeAtLocation(Vec2(i, j));

      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dy == 0 && dx == 0)
            continue;  // Skip the current cell because its our cell
          int neighborY = i + dy;
          int neighborX = j + dx;
          if (neighborY >= 0 && neighborY < MAP_SIZE && neighborX >= 0 &&
              neighborX < MAP_SIZE) {
            neighbors.push_back(&GetNodeAtLocation(Vec2(neighborX, neighborY)));
          }
        }
      }
      nodes[temp->location] = *temp;
      nodes[temp->location].AddNeighbors(neighbors);
    }
  }
}
Graph::Graph() {}

std::vector<Node *> Graph::GetAllGraphNodes() {
  std::vector<Node *> result;
  for (auto it = nodes.begin(); it != nodes.end(); it++) {
    result.push_back(&it->second);
  }
  return result;
}

std::vector<Node *> Graph::GetAllNeighbors(Node &n) {
  return n.neighbors;
}

Node &Graph::GetNodeAtLocation(const Vec2 &loc) { return nodes[loc]; }
