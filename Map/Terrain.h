#ifndef TERRAIN_H
#define TERRAIN_H
#include <memory>
#include <vector>

#include "../Tools/Vec2.h"  

#define MAP_SIZE 5
enum TerrainType { GROUND, GOLD };

class Living;
class Structure;

class Terrain {
 public:
  Terrain(int x, int y) {
    coord.x = x;
    coord.y = y;
    resourceLeft = 0;
  }

  bool operator==(const Terrain& other) const;

 public:
  int GetResourceLeft() { return 0; }
  // std::unique_ptr<i><Living> ObjectsAtTerrain(Vec2 terr);
  bool ExistsInTerrain(Living &);

 public:
  int resourceLeft{};
  Vec2 coord;
  TerrainType type = GROUND;
  std::vector<Living *> onTerrainLiving;
  Structure *structureOnTerrain = nullptr;
};
#endif
