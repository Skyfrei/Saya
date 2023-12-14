//
// Created by Klavio Tarka on 14.12.23.
//

#include "Task.h"
namespace Warcraft::State
{
    template<typename X>
    Task<X>::Task(Living& ref)
    {
        percentage = 0.0f;
        livingReference = ref;
    }

    template<typename X>
    bool Task<X>::IsFinished()
    {

    }

    template<typename X>
    void Task<X>::UpdateTask(float progress)
    {
        percentage += progress;
    }
}