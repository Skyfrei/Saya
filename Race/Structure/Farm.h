#pragma once
#include "Structure.h"
#include <string>

using namespace Warcraft::Structures;

class Farm : public Structure
{
    public:
        Farm()
        {
            name = "Farm";
            description = "Gives 6 food.";
            health = 500;
            maxHealth = health;

            goldCold = 100;

            buildTime = 10;
        }
    public:
        std::string GetDescription() override {return description;}

    public: 
        int GetFood() { return 5;}


}