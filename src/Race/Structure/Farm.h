#pragma once
#include <string>

#include "Structure.h"

class Farm : public Structure {
 public:
  Farm(Vec2 coord);
    Farm(Vec2 coord, float hp);
 public:
    std::string GetDescription() override;
  void FinishBuilding() override;
  std::unique_ptr<Structure> Clone() const;

 public:
  int GetFood();
};
