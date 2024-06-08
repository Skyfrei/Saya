#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../../Living.h"
enum StructureType { HALL, BARRACK, FARM, NR_OF_STRUCTS, OTHER };

class Structure : public Living {
 public:
  Structure() {}
  virtual std::unique_ptr<Structure> Clone() const = 0;
  virtual void FinishBuilding() = 0;
  bool operator==(const Structure& other);

 public:
  StructureType is = OTHER;
  bool isBeingBuilt = false;
};

#endif
