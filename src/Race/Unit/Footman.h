#ifndef FOOTMAN_H
#define FOOTMAN_H

#include "../../Tools/Vec2.h"
#include "Unit.h"
#include <string>

class Footman : public Unit
{
  public:
    Footman();

    std::string GetDescription() override;
    Unit* Clone() const override;
    Footman(Vec2 coord, float hp, float man);
};
#endif
