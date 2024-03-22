#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "../Tools/Vec2.h"
#include "Graph.h"
#include "Terrain.h"

class Map {
 public:
  Map();

  void TurnMatrixToGraph();

 public:
  std::vector<Node> GetClosestDestNode(Vec2 &coord, Vec2 &dest);
  std::vector<std::vector<Terrain>> objects;

 private:
  std::vector<Node> GetAllNodes();
  Graph graph;
};

#endif
