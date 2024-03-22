#ifndef GRAPH_H
#define GRAPH_H
#include <ctime>
#include <map>
#include <vector>

#include "../Tools/Vec2.h"
#include "Terrain.h"

struct Node {
  std::vector<Node *> neighbors;
  Vec2 location;

  Node(std::vector<Node *> e) { neighbors = e; }
  Node(Vec2 l) { location = l; }
  void AddNeighbors(std::vector<Node *> e) { neighbors = e; }
  Node() {}
  constexpr bool operator<=>(const Node &n1) const {
    return location == n1.location;
  }
  constexpr bool operator!=(const Node &n1) const {
    return location != n1.location;
  }
  constexpr bool operator==(const Node &n1) const {
    return location == n1.location;
  }
};

class Graph {
 public:
  Graph(std::vector<std::vector<Terrain>> m);
  Graph();
  std::vector<Node> GetAllGraphNodes();
  std::vector<Node *> GetAllNeighbors(Node &n);
  Node GetNodeAtLocation(const Vec2 &loc);

 private:
  std::map<Vec2, Node> nodes;
};
#endif
