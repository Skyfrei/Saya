#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "../../Living.h"
#include "../../Tools/Enums.h"
#include <memory>
#include <string>

class Structure : public Living {
    public:
        Structure() {}
        virtual std::unique_ptr<Structure> Clone() const = 0;
        virtual void FinishBuilding() = 0;
        bool operator==(const Structure& other);
        std::string Serialize();


    public:
        StructureType is = OTHER;
        bool isBeingBuilt = false;
};

#endif
