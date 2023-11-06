#include "Manager.h"

void Manager::MainLoop()
{   
    
}

void Manager::CheckForMovement()
{

}

float Manager::GetTime()
{
    //return end - start;
    float time = std::chrono::steady_clock::now() - start;
    return time;
}