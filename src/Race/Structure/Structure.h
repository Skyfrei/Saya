#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../../Living.h"
#include "../../Tools/Binary.h"
#include "../../Tools/Enums.h"
#include <memory>
#include <string>
#include <vector>

class Structure : public Living
{
  public:
    Structure() {
    }
    virtual Structure* Clone() const = 0;
    virtual void FinishBuilding() = 0;
    bool operator==(const Structure &other);
    std::string Serialize();
    std::vector<binary> SerializeBinary();
    // Structure* DeserializeBinary(std::vector<binary>& bin);

  public:
    StructureType is = OTHER;
    bool isBeingBuilt = false;
};

#endif
