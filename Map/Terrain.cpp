#include "Terrain.h"
#include "../Race/Unit/Unit.h"

bool Terrain::ExistsInTerrain(Living &l) {
  for (int i = 0; i < onTerrainLiving.size(); i++) {
    if (onTerrainLiving[i] == &l) {
      return true;
    }
  }
  return false;
}
bool Terrain::operator==(const Terrain& other) const {
  if (other.resourceLeft != resourceLeft ||
      structureOnTerrain != other.structureOnTerrain ||
      other.onTerrainLiving.size() != onTerrainLiving.size() ){
    return false;
  }
  for (int i = 0; i < onTerrainLiving.size(); i++){
    Unit* unit = dynamic_cast<Unit*>(onTerrainLiving[i]);
    Unit* otherUnit = dynamic_cast<Unit*>(other.onTerrainLiving[i]);
    if (*unit != *otherUnit){
      return false;
    }
  }
  return true;
}
