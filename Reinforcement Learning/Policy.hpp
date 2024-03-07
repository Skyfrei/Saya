#ifndef POLICY_H
#define POLICY_H

#include <iostream>
#include "IAction.hpp"
#include "../Map/Map.h"


class Policy{

  public:
    Policy();
    ~Policy() = default;

  public:


  private:
    float gamma = 0.05f; // Future reward discount
    IAction actions;
    Map map;
};
#endif