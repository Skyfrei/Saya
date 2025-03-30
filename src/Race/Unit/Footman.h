#ifndef FOOTMAN_H
#define FOOTMAN_H

#include <string>
#include "Unit.h"
#include "../../Tools/Vec2.h"

class Footman : public Unit {
    public:
        Footman();

        std::string GetDescription() override;
        std::unique_ptr<Unit> Clone() const override;
        Footman(Vec2 coord, float hp, float man);

};
#endif
