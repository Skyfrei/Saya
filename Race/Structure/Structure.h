#pragma once
#include "../../Tools/Vec2.h"
#include "../../Living.h"
#include <queue>
#include "../../State/Task.h"



namespace Warcraft::Structures
{
    using Warcraft::Living;
    using namespace Warcraft::State;

    enum StructureType
    {
        HALL,
        BARRACK,
        FARM,
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
            std::queue<Task<Structure>> tasks;
            bool isBeingBuilt = false;

            
            // std::vector<> building contained within building
    };
}
