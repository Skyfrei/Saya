#pragma once

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "Player.h"

class Game
{
    public: 
        Game()
        {

        }
    
    public:
        float GetTime()
        {
            //return end - start;
            float time = std::chrono::steady_clock::now() - start;
            return time;
        }
        void Initialize()
        {

        }
    
    private:
        auto start = std::chrono::steady_clock::now();
        //clock_t start, end;
};