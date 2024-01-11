#include "../Map/Map.h"


class Policy
{
    using Warcraft::Environment;

    enum Action
    {
        BUILD_HALL
    };


    public:
        Policy(Map& m);

        int TakeAction(Action act); // Based on the map do shit
        
    private:
        int GetReward();
        Map& map; // this would be the state of the game
};


