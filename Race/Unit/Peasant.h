#ifndef PEASANT_H
#define PEASANT_H
#include "Unit.h"
#include "../Structure/Structure.h"
#include "../Structure/TownHall.h"
#include "../Structure/Barrack.h"
#include "../Structure/Farm.h"
#include "../../Map/Map.h"
#include "../../Map/Terrain.h"
#include <vector>
#include <deque>
#include <memory>



namespace Warcraft::Units
{
    using namespace Warcraft::Structures;
    using namespace Warcraft::Environment;
    using namespace Warcraft::State;

    class Peasant : public Unit
    {
        public:
            Peasant()
            {
                name = "Peasant";
                description = "God damn farm workers.";
                health = 240;
                maxHealth = health;

                attack = 5.5;
                
                mana = 200;
                manaRegen = 0.67f;

                goldCost = 75;
                foodCost = 1;

                attackCooldown = 2.0f;
                buildTime = 15;

                is = PEASANT;
            }

        public:
            void Build(std::vector<std::shared_ptr<Structure>>& structures, int& playerGold, StructureType type, Terrain& terr);
            void WorkOnTask();
            void ChangeBuilding();
            void FarmGold(Terrain& terr, TownHall& hall);
            void TransferGold(TownHall& hall);
            std::string GetDescription() override{return "Slave.";}

        public:
            int goldInventory = 0;
        private:
            std::deque<Task<Structure&>> buildTask;
    };
}

#endif