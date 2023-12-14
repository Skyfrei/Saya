//
// Created by Klavio Tarka on 14.12.23.
//

#ifndef FINAL_TASK_H
#define FINAL_TASK_H

#include "../Living.h"

namespace Warcraft::State
{
    using namespace Warcraft;

    template<typename X>
    class Task {
        public:
            Task(Living& ref);
            bool IsFinished();
            ///Parameter progress could be negative too. Soldiers attacking an unfinished building do negative to the progress.
            void UpdateTask(float progress);

        public:
            float percentage = 0.0f;
            static int id;
            Living& livingReference;
    };
}


#endif //FINAL_TASK_H
