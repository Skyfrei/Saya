#include "Terrain.h"

bool Terrain::ExistsInTerrain(Living &l) {
  for (int i = 0; i < onTerrainLiving.size(); i++) {
    if (onTerrainLiving[i] == &l) {
      return true;
    }
  }
  return false;
}
