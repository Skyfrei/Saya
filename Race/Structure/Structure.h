#pragma once
#include "../../Tools/Vec2.h"
#include "../../Living.h"

using Warcraft::Living;

namespace Warcraft::Structures
{
    enum StructureType
    {
        HALL,
        BARRACK,
        OTHER
    };

    class Structure : public Living
    {
        public:
            Structure()
            {
                
            }

        public:
            virtual void FinishBuilding() = 0;

        public:
            StructureType is = OTHER;

            
            // std::vector<> building contained within building
    };
}
