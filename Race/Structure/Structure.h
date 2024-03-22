#pragma once
#include <queue>

#include "../../Living.h"
#include "../../Tools/Vec2.h"

enum StructureType { HALL, BARRACK, FARM, OTHER };

class Structure : public Living {
 public:
  Structure() {}

 public:
  virtual void FinishBuilding() = 0;

 public:
  StructureType is = OTHER;
  // std::queue<Task<Structure>> tasks;
  bool isBeingBuilt = false;

  // std::vector<> building contained within building
};
