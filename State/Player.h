#ifndef PLAYER_H
#define PLAYER_H

#include "../Tools/Vec2.h"
#include <memory>
#include <vector>
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Peasant.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Unit/Unit.h"
#include <memory>
#include "../Living.h"
#include "../Map/Map.h"

namespace Warcraft::State
{
    using namespace Warcraft::Units;
    using namespace Warcraft::Structures;
    using namespace Warcraft;
    using namespace Warcraft::Environment;

    class Player
    {
        public: 
            Player();
            void Initialize(Map& m);
            void SetInitialCoordinates(Vec2 v);
            bool HasStructure(StructureType structType);
            bool HasUnit(UnitType unitType);
            Structure& FindClosestStructure(Unit& unit, StructureType type);
            Living& Select();
            void ValidateFood();
            void UpdateGold(int g);
            void ChooseToBuild(StructureType structType, Vec2 terrCoord);
            void RecruitSoldier(UnitType unitType);

        public:
            int gold;
            Vec2 food;
            std::vector<std::unique_ptr<Unit>> units;
            std::vector<std::unique_ptr<Structure>> structures;
        private:
            Map map;
    };
}

#endif

