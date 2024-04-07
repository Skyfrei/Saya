#ifndef TERRAIN_H
#define TERRAIN_H
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
  // std::unique_ptr<i><Living> ObjectsAtTerrain(Vec2 terr);
  bool ExistsInTerrain(Living &);

 public:
  int resourceLeft{};
  Vec2 coord;
  TerrainType type = GROUND;
  std::vector<Living *> onTerrainLiving;
};
#endif
