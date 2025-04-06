#ifndef TOWNHALL_H
#define TOWNHALL_H
#include "Structure.h"

enum Upgrade
{
    WOOD,
    BRONZE,
    IRON,
    STEEL,
    MITHRIL
};

class TownHall : public Structure
{
  public:
    TownHall(Vec2 coord);
    TownHall(Vec2 coord, float hp);

  public:
    Upgrade currentUpgrade = WOOD;
    void UpgradeEquipment(Upgrade &curr);
    void FinishBuilding() override;
    std::unique_ptr<Structure> Clone() const override;
    std::string GetDescription();
};

#endif
