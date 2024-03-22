#pragma once
#include <memory>

#include "../Living.h"
#include "../Tools/Vec2.h"

#define MAP_SIZE 15

enum TerrainType { GROUND, GOLD };

class Terrain {
 public:
  Terrain(int x, int y) {
    coord.x = x;
    coord.y = y;
  }

 public:
  int GetResourceLeft() { return 0; }

  // void GetLiving(std::vector<Living>& obj)
  //{
  //  objects = obj;
  //}

 public:
  int resourceLeft{};
  Vec2 coord;
  TerrainType type = GROUND;
  std::shared_ptr<Living> object;
};
