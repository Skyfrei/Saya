#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../../Living.h"
#include "../../Tools/Enums.h"
#include "../../Tools/Binary.h"
#include <vector>
#include <string>
#include <memory>

class Structure : public Living {
    public:
        Structure() {}
        virtual std::unique_ptr<Structure> Clone() const = 0;
        virtual void FinishBuilding() = 0;
        bool operator==(const Structure& other);
        std::string Serialize();
        std::vector<binary> SerializeBinary();
        Structure* DeserializeBinary(std::vector<binary>& bin);

    public:
        StructureType is = OTHER;
        bool isBeingBuilt = false;
};

#endif
