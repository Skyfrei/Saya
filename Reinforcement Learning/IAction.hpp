#ifndef IACTION_H
#define IACTION_H

#include <iostream>
#include <vector>


class IAction{
  public:
    void virtual Attack() = 0;
    void virtual Harvest() = 0;
    void virtual Build() = 0;
    void virtual Recruit() = 0;
    void virtual Cast() = 0; // For heroes
    void virtual Use() = 0; // For heroes
    void virtual Move(Vec2 terr) = 0;
};
#endif